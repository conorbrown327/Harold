/** CSci-4611 Assignment 6: Harold
 */

#include "sky.h"


Sky::Sky() {
    
}


Sky::~Sky() {
    
}

void Sky::Init(ShaderProgram *stroke3d_shaderprog) {
    stroke3d_shaderprog_ = stroke3d_shaderprog;
}


/// Projects a 2D normalized screen point (e.g., the mouse position in normalized
/// device coordinates) to a 3D point on the "sky", which is really a huge sphere
/// (radius = 1500) that the viewer is inside.  This function should always return
/// true since any screen point can successfully be projected onto the sphere.
/// sky_point is set to the resulting 3D point.  Note, this function only checks
/// to see if the ray passing through the screen point intersects the sphere; it
/// does not check to see if the ray hits the ground or anything else first.
bool Sky::ScreenPtHitsSky(const Matrix4 &view_matrix, const Matrix4 &proj_matrix,
                        const Point2 &normalized_screen_pt, Point3 *sky_point)
{
    Point3 eye = view_matrix.Inverse() * Point3(0, 0, 0);
    Point3 mouseIn3d = GfxMath::ScreenToNearPlane(view_matrix, proj_matrix, normalized_screen_pt);
    Ray eyeThroughMouse = Ray(eye, mouseIn3d - eye);
    float iTime = 0.0;

    bool intersects = eyeThroughMouse.IntersectSphere(Point3(0, 0, 0), 1500.0, &iTime, sky_point);
    return true;
}




/// Creates a new sky stroke mesh by projecting each vertex of the 2D mesh
/// onto the sky dome and saving the result as a new 3D mesh.
void Sky::AddSkyStroke(const Matrix4 &view_matrix, const Matrix4 &proj_matrix,
                       const Mesh &stroke2d_mesh, const Color &stroke_color)
{
    // create a new mesh too loop through and edit vertices
    Mesh m = stroke2d_mesh;
    std::vector<Point3> new_verts;

    // loop through verticies and project each on the sky dome
    for (int i = 0; i < m.num_vertices(); i++)
    {
        Point3 *point_on_sphere = new Point3();
        Point3 vertex_i = m.read_vertex_data(i);
        Point2 p2_vertex_i;
        p2_vertex_i[0] = vertex_i[0];
        p2_vertex_i[1] = vertex_i[1];
        ScreenPtHitsSky(view_matrix, proj_matrix, p2_vertex_i, point_on_sphere);
        new_verts.push_back(*point_on_sphere);
    }
    // change the verticies of m to the projected verticies
    m.SetVertices(new_verts);

    SkyStroke ss;
    ss.mesh = m;
    ss.color = stroke_color;
    // add the sky stroke to the vecotr of sky strokes
    strokes_.push_back(ss);
}


/// Draws all of the sky strokes
void Sky::Draw(const Matrix4 &view_matrix, const Matrix4 &proj_matrix) {

    // Precompute matrices needed in the shader
    Matrix4 model_matrix; // identity
    Matrix4 modelview_matrix = view_matrix * model_matrix;
    
    // Draw sky meshes
    stroke3d_shaderprog_->UseProgram();
    stroke3d_shaderprog_->SetUniform("modelViewMatrix", modelview_matrix);
    stroke3d_shaderprog_->SetUniform("projectionMatrix", proj_matrix);
    for (int i=0; i<strokes_.size(); i++) {
        stroke3d_shaderprog_->SetUniform("strokeColor", strokes_[i].color);
        strokes_[i].mesh.Draw();
    }
    stroke3d_shaderprog_->StopProgram();
}

