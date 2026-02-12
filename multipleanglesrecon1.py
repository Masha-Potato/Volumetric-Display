import trimesh
from shapely.geometry import Polygon, LineString
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from skimage.draw import polygon as sk_polygon

def slice_mesh(mesh, plane_origin, plane_normal):
    """
    Slice the mesh with a plane and return a 3D cross-section as a Path3D.
    """
    return mesh.section(plane_origin=plane_origin, plane_normal=plane_normal)

def generate_cross_sections(mesh, num_angles=120, plane_origin=None):
    
    """
    Generate cross-sections of the mesh at multiple angles around Z-axis.
    """
    if plane_origin is None:
        plane_origin = mesh.centroid

    angles = np.linspace(0, 360, 360, endpoint=False)
    sections = []

    for angle in angles:
        theta = np.radians(angle)
        normal = np.array([np.cos(theta), np.sin(theta), 0])
        path_3d = slice_mesh(mesh, plane_origin, normal)
        sections.append((path_3d))

    return sections

def plot_cross_sections_3d(sections):
    """
    Plot all cross-sections as filled surfaces in a 3D plot.
    """
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection='3d')

    face_color = 'red'
    alpha = 0.6

    all_verts = []
    print("all verts shape: ")
    print(np.shape(all_verts))
    for i in range(len(sections)):
        if sections[i] is None:
            continue

        # Convert to 2D planar path and transform
        try:
            path_2d, to_3D = sections[i].to_planar()
        except Exception:
            continue
        #CHANGE TEMP TO LIST
        #switch between 0, 2 and 0, 3 as needed
        temp = np.empty((0, 2))
        
        for polygon in path_2d.polygons_full:
            #print(i)
            #print(",")
            #print(len(polygon.exterior.coords))
            if polygon is None or len(polygon.exterior.coords) < 5:
                continue

            # Convert 2D polygon back to 3D using the transform
            verts_2D = np.column_stack((polygon.exterior.coords.xy))
            verts_2D_hom = np.hstack([verts_2D, np.zeros((len(verts_2D), 1)), np.ones((len(verts_2D), 1))])
            verts_3D = (to_3D @ verts_2D_hom.T).T[:, :3]

            #switch between verts_2D and verts_3D as needed
            #CHANGE THIS TO JUST APPEND
            temp = np.vstack((temp, verts_2D))

            ax.add_collection3d(Poly3DCollection(
                [verts_3D],
                facecolors=face_color,
                edgecolors='black',
                linewidths=0.5,
                alpha=alpha
            ))
        #print("temp shape: ")
        #print(np.shape(temp))
        all_verts.append(temp)
        #print("all_verts shape: ")
        #print(np.shape(all_verts))


    ax.set_title("Filled 3D Cross Sections")
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.auto_scale_xyz(*np.array(mesh.bounds).T)
    print("mesh bounds: ")
    print(mesh.bounds)
    print(np.min(mesh.bounds[:,:2]))#xmin
    print(mesh.bounds[0][2])#y min (actually z but whatever)
    print(np.max(np.sum(np.abs(mesh.bounds), axis=0)))#scale to divide for both x and y
    #print(np.max(np.sum(mesh.bounds, axis=0)))
    print(range(5, 505, 2))
    ax.view_init(elev=20, azim=45)
    plt.tight_layout()
    plt.show()
    
    all_verts = np.array(all_verts, dtype=object)
    print(np.shape(all_verts))
    return all_verts

def project_polygon_to_2D(pts3D):
    # pts3D: (N, 3) numpy array of polygon vertices
    center = pts3D.mean(axis=0)
    normal = np.cross(pts3D[1] - pts3D[0], pts3D[2] - pts3D[0])
    normal /= np.linalg.norm(normal)

    # Create orthonormal basis (u, v) in the plane
    u = pts3D[1] - pts3D[0]
    u /= np.linalg.norm(u)
    v = np.cross(normal, u)

    # Project points into 2D
    local_coords = np.dot(pts3D - center, np.vstack([u, v]).T)
    return local_coords  # shape (N, 2)

def rasterize_polygon(coords_2D, size=256):
    x, y = coords_2D.T
    print(coords_2D.T)
    # Normalize to fit in image space
    x = (x - x.min()) / (x.max() - x.min() + 1e-8) * (size - 1)
    y = (y - y.min()) / (y.max() - y.min() + 1e-8) * (size - 1)
    rr, cc = sk_polygon(x, y)  # y is row axis
    print("rr:")
    print(type(rr))
    print(rr)
    print(cc)
    img = np.zeros((size, size), dtype=np.uint8)
    rr = np.clip(rr, 0, size - 1)
    cc = np.clip(cc, 0, size - 1)
    img[rr, cc] = 1
    return img

if __name__ == "__main__":
    stl_path = "model.stl"
    mesh = trimesh.load_mesh(stl_path)

    # Compute cross-sections
    sections = generate_cross_sections(mesh, num_angles=120)

    # Show in 3D
    all_faces = plot_cross_sections_3d(sections) # list of arrays, each face is an array of 3D points

    for face in all_faces:
        #poly = np.array(face)            # (N, 3)
        #print(np.shape(poly))
        #poly_2D = project_polygon_to_2D(poly)  # (N, 2)
        #print(poly_2D)
        #image = rasterize_polygon(poly_2D, size=256)
        image = rasterize_polygon(np.array(face), size=256)
        fig, axes = plt.subplots(1, 1, figsize=(16, 4))
        axes.imshow(image, cmap='gray')
        plt.tight_layout()
        plt.show()



