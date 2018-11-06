// @todo : gizmo

#pragma once

#include "../engine/math/math.c"
#include "../engine/render/render_shape.c"

#define X(v3) 0[v3]
#define Y(v3) 1[v3]
#define Z(v3) 2[v3]
#define W(v4) 3[v4]

void vec3_set( vec3 v, float x, float y, float z ) {
    0[v] = x, 1[v] = y, 2[v] = z;
}
void vec3_cpy( vec3 v, vec3 b ) {
    memcpy(v,b, sizeof(vec3));
}

static
void RotationPitchYawRoll( mat4 m, const float pitch, const float yaw, const float roll )
{
    float cp = (float)cos(-pitch);
    float sp = (float)sin(-pitch);

    float cy = (float)cos(yaw);
    float sy = (float)sin(yaw);

    float cr = (float)cos(-roll);
    float sr = (float)sin(-roll);

    float sysp = sy * sp;
    float sycp = sy * cp;

    m[0][0] = cr * cy;
    m[0][1] = sr * cy;
    m[0][2] = -sy;
    m[0][3] = 0;
    m[1][0] = cr * sysp - sr * cp;
    m[1][1] = sr * sysp + cr * cp;
    m[1][2] = cy * sp;
    m[1][3] = 0;
    m[2][0] = cr * sycp + sr * sp;
    m[2][1] = sr * sycp - cr * sp;
    m[2][2] = cy * cp;
    m[2][3] = 0;
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}
static
void euler_to_quat_(quat q, float pitch, float yaw, float roll) {
    float cr = (float)cos(-roll * 0.5);
    float sr = (float)sin(-roll * 0.5);
    float cp = (float)cos(-pitch * 0.5);
    float sp = (float)sin(-pitch * 0.5);
    float cy = (float)cos(yaw * 0.5);
    float sy = (float)sin(yaw * 0.5);

    X(q) = cr * sp * cy - sr * cp * sy;
    Y(q) = cr * cp * sy + sr * sp * cy;
    Z(q) = sr * cp * cy - cr * sp * sy;
    W(q) = cr * cp * cy + sr * sp * sy;
}
// A coordinate transform performs the transformation with the assumption that the w component
// is one. The four dimensional vector obtained from the transformation operation has each
// component in the vector divided by the w component. This forces the w component to be one and
// therefore makes the vector homogeneous. The homogeneous vector is often preferred when working
// with coordinates as the w component can safely be ignored.
static
void TransformCoordinate( vec3 result, vec3 coordinate, mat4 transform ) {
    vec4 v, w = { coordinate[0], coordinate[1], coordinate[2], 1 };
    mat4_mul_vec4( v, transform, w );
    result[0] = v[0];
    result[1] = v[1];
    result[2] = v[2];
}

namespace im3d {
    struct Camera {
        vec3 Position;
        vec3 Target;
    };
    struct Viewport {
        float x, y, w, h;
        // --
        float yfovdeg, znear, zfar;
        Viewport() : x(0), y(0), w(640), h(480), yfovdeg(45), znear(0.01), zfar(100.f)
        {}
    };
    struct Face {
        int A;
        int B;
        int C;
    };
    struct Mesh {
        const char *Name;
        vec3 *Vertices; int VerticesCount;
        Face *Faces;    int FacesCount;
        vec3 Position;
        vec3 Rotation;
        vec3 Scale;
        vec3 AABB[2], midpoint, extent;

        Mesh(const char *name, int facesCount, int verticesCount) : Name(name), VerticesCount(verticesCount), FacesCount(facesCount) {
            Vertices = (vec3*)malloc( sizeof(vec3) * verticesCount );
            Faces = (Face*)malloc( sizeof(Face) * facesCount );
            vec3_set(Position, 0, 0, 0);
            vec3_set(Rotation, 0, 0, 0);
            vec3_set(Scale,    1, 1, 1);
            compute_AABB();
        }
        Mesh(const char *name, int facesCount, int verticesCount, const int *idx, const float *vtx) : Name(name), VerticesCount(verticesCount), FacesCount(facesCount) {
            Vertices = (vec3*) malloc(sizeof(vec3) * verticesCount);
            Faces = (Face*) malloc(sizeof(Face) * facesCount);
            vec3_set(Position, 0, 0, 0);
            vec3_set(Rotation, 0, 0, 0);
            vec3_set(Scale,    1, 1, 1);
            if (vtx) for (int i = 0; i < verticesCount; ++i, vtx += 3) {
                vec3_set( Vertices[i], vtx[0], vtx[1], vtx[2]);
            }
            if( idx ) for( int i = 0; i < facesCount; ++i, idx += 3 ) {
                Faces[i] = Face { idx[0], idx[1], idx[2] };
            }
            compute_AABB();
        }
        ~Mesh() {
            free( Faces );
            free( Vertices );
        }
        void compute_AABB() {
            vec3_set( AABB[0], +FLT_MAX,+FLT_MAX,+FLT_MAX );
            vec3_set( AABB[1], -FLT_MAX,-FLT_MAX,-FLT_MAX );
            for (int i = 0; i < VerticesCount; ++i) {
                if( X(Vertices[i]) < X(AABB[0]) ) X(AABB[0]) = X(Vertices[i]);
                if( X(Vertices[i]) > X(AABB[1]) ) X(AABB[1]) = X(Vertices[i]);

                if( Y(Vertices[i]) < Y(AABB[0]) ) Y(AABB[0]) = Y(Vertices[i]);
                if( Y(Vertices[i]) > Y(AABB[1]) ) Y(AABB[1]) = Y(Vertices[i]);

                if( Z(Vertices[i]) < Z(AABB[0]) ) Z(AABB[0]) = Z(Vertices[i]);
                if( Z(Vertices[i]) > Z(AABB[1]) ) Z(AABB[1]) = Z(Vertices[i]);
            }

            vec3 v1;
            vec3_set( v1,
                0.5f * (X(AABB[0]) + X(AABB[1])),
                0.5f * (Y(AABB[0]) + Y(AABB[1])),
                0.5f * (Z(AABB[0]) + Z(AABB[1])));
            vec3_cpy( midpoint, v1 );

            vec3 v2;
            vec3_set( v2, X(AABB[1]) - X(AABB[0]), Y(AABB[1]) - Y(AABB[0]), Z(AABB[1]) - Z(AABB[0]) );
            vec3_cpy( extent, v2 );
            for( int i = 0; i < 3; ++i) extent[i] *= (extent[i] < 0 ? -1 : 1);
        }
    };

    // Project 3D coordinates as 2D point using the transformation matrix
    // where transformMatrix = worldMatrix * viewMatrix * projectionMatrix;
    void Project(vec2 p2, vec3 p3, mat4 transformMatrix, const Viewport *vp) {
        // transforming the coordinates
        vec2 point;
        TransformCoordinate(point, p3, transformMatrix);

        // The transformed coordinates will be based on coordinate system
        // starting on the center of the screen. But drawing on screen normally starts
        // from top left. We then need to transform them again to have x:0, y:0 on top left.
        X(p2) = ( X(point) * vp->w + vp->w / 2.0f ) + vp->x;
        Y(p2) = (-Y(point) * vp->h + vp->h / 2.0f ) + vp->y;
    }

    const ImU32 red = IM_COL32(255,0,0,64), purple = IM_COL32(128,0,128,128);

    // re-compute each vertex projection
    void Render(Camera camera, const Viewport *vp, int num_meshes, Mesh *meshes, ImU32 color = purple)
    {
        vec3 up; vec3_set(up, 0,1,0);
        mat4 viewMatrix;
        mat4_look_at(viewMatrix, camera.Position, camera.Target, up );

        mat4 projectionMatrix;
        mat4_perspective(projectionMatrix, (vp->yfovdeg*2) * 0.0174533f, vp->w / vp->h, vp->znear, vp->zfar );

/*
        glm::vec3 Dir = glm::normalize(CameraGetDirection(Camera));
        glm::vec3 Up = glm::normalize(CameraGetUp(Camera));

        Camera->View = glm::lookAt(
            Pos,
            Pos+Dir,
            Up);

        FrustumExtract(Camera->Projection * Camera->View);
*/

        int   segments = 4;
        float radius = 5.f;
        float thickness = 1; // 6.f;
        ImU32 dotcolor = (ImGui::GetColorU32(ImGuiCol_PlotLinesHovered) & IM_COL32(255,255,255,0)) | IM_COL32(0,0,0,64);
        auto *drawlist = ImGui::GetWindowDrawList();

        for( int m = 0; m < num_meshes; ++m ) {
            Mesh *mesh = &meshes[m];

#if 0
            // apply rotation before translation
            mat4 worldMatrix, temp;
            RotationPitchYawRoll(temp, X(mesh->Rotation) * 0.0174533f, Y(mesh->Rotation) * 0.0174533f, Z(mesh->Rotation) * 0.0174533f);
            mat4_scale_aniso(worldMatrix, temp, X(mesh->Scale),Y(mesh->Scale),Z(mesh->Scale));
            mat4_translate_in_place(worldMatrix, X(mesh->Position), Y(mesh->Position), Z(mesh->Position));
#else
            // scale mtx
            mat4 worldMatrix, temp, temp2;
            mat4_identity(worldMatrix);
            mat4_scale_aniso(temp2, worldMatrix, X(mesh->Scale),Y(mesh->Scale),Z(mesh->Scale));

            // apply rotation before translation
            #if 0 // ok
              RotationPitchYawRoll(temp, X(mesh->Rotation) * 0.0174533f, Y(mesh->Rotation) * 0.0174533f, Z(mesh->Rotation) * 0.0174533f);
            #else
              quat q; euler_to_quat_( q, X(mesh->Rotation) * 0.0174533f, Y(mesh->Rotation) * 0.0174533f, Z(mesh->Rotation) * 0.0174533f );
              mat4_from_quat( temp, q );
            #endif
            mat4_mul(worldMatrix, temp, temp2);

            mat4_translate_in_place(worldMatrix, X(mesh->Position), Y(mesh->Position), Z(mesh->Position));
#endif

            // transformMatrix = worldMatrix * viewMatrix * projectionMatrix;
            mat4 transformMatrix;
            mat4_mul(temp, projectionMatrix, viewMatrix);
            mat4_mul(transformMatrix, temp, worldMatrix);

            if(1)
            for( int f = 0; f < mesh->FacesCount; ++f )
            {
                Face *face = &mesh->Faces[f];

                vec2 pixelA; Project( pixelA, mesh->Vertices[face->A], transformMatrix, vp );
                vec2 pixelB; Project( pixelB, mesh->Vertices[face->B], transformMatrix, vp );
                vec2 pixelC; Project( pixelC, mesh->Vertices[face->C], transformMatrix, vp );

                drawlist->AddLine( ImVec2(X(pixelA),Y(pixelA)), ImVec2(X(pixelB),Y(pixelB)), color, thickness );
                drawlist->AddLine( ImVec2(X(pixelB),Y(pixelB)), ImVec2(X(pixelC),Y(pixelC)), color, thickness );
                drawlist->AddLine( ImVec2(X(pixelC),Y(pixelC)), ImVec2(X(pixelA),Y(pixelA)), color, thickness );
            }

            if(1)
            for( int v = 0; v < mesh->VerticesCount; ++v )
            {
                vec2 point;
                Project( point, mesh->Vertices[v], transformMatrix, vp );

                //drawlist->AddCircle( ImVec2(X(point),Y(point)), radius, color, segments, thickness );
                drawlist->AddRect( ImVec2(X(point)-2,Y(point)-2), ImVec2(X(point)+2,Y(point)+2), dotcolor );
            }
        }
    }
}



void im3d_demo() {
    using namespace im3d;
    static Camera cam = { {0, 0, 90}, {0,0,0} };
    static Viewport vp;
    static bool rotate[3] = {0,0,0};

    static int selected = 0;
    static Mesh *prims[] = {
        #define SHAPE_NEW_MESH(prim,a,b) new Mesh(#prim,shape_##prim##_face_count,shape_##prim##_vertex_count,shape_##prim##_face,shape_##prim##_vertex),
        SHAPE_XMACRO(SHAPE_NEW_MESH)
    };

    if( ImGui::Begin("im3d") ) {
        auto xy = ImGui::GetWindowPos();
        auto wh = ImGui::GetWindowSize();
        vp.x = xy.x, vp.y = xy.y, vp.w = wh.x, vp.h = wh.y;

        Mesh &prim = *prims[selected];
        Render( cam, &vp, 1, &prim );
        vec3_set( prim.Rotation, X(prim.Rotation) + 0.5f * rotate[0], Y(prim.Rotation) + 0.5f * rotate[1], Z(prim.Rotation) + 0.5f * rotate[2] );

        {
            static Mesh cb = Mesh("cube", shape_cube_face_count, shape_cube_vertex_count, shape_cube_face, shape_cube_vertex);

            vec3_set( cb.Position,  X(prim.midpoint) - X(cb.midpoint), Y(prim.midpoint) - Y(cb.midpoint), Z(prim.midpoint) - Z(cb.midpoint) );
            vec3_cpy( cb.Rotation, prim.Rotation );
            vec3_set( cb.Scale,  prim.extent[0]/2, prim.extent[1]/2, prim.extent[2]/2 );

            Render(cam, &vp, 1, &cb, red );
        }

        ImGui::SliderInt("model", &selected, 0, (sizeof(prims)/sizeof(prims[0])) - 1);
        ImGui::SameLine(); ImGui::TextDisabled( prim.Name );
        ImGui::SliderFloat("fov deg", &vp.yfovdeg, 0, 90);
        ImGui::SliderFloat("X", &X(prim.Rotation), -180, 180); ImGui::SameLine(); ImGui::Checkbox("rotate X", &rotate[0]); ImGui::SameLine(); if( ImGui::SmallButton("\xEE\x85\xA6##1"/*ICON_MD_UNDO*/) ) prim.Rotation[0] = 0, rotate[0] = 0;
        ImGui::SliderFloat("Y", &Y(prim.Rotation), -180, 180); ImGui::SameLine(); ImGui::Checkbox("rotate Y", &rotate[1]); ImGui::SameLine(); if( ImGui::SmallButton("\xEE\x85\xA6##2"/*ICON_MD_UNDO*/) ) prim.Rotation[1] = 0, rotate[1] = 0;
        ImGui::SliderFloat("Z", &Z(prim.Rotation), -180, 180); ImGui::SameLine(); ImGui::Checkbox("rotate Z", &rotate[2]); ImGui::SameLine(); if( ImGui::SmallButton("\xEE\x85\xA6##3"/*ICON_MD_UNDO*/) ) prim.Rotation[2] = 0, rotate[2] = 0;
    }
    ImGui::End();
}
