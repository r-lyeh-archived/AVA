// This file is mostly @sgorsten's work ported back from modern C++ to C. Original code is Unlicensed.
// - rlyeh, public domain.

#ifndef MESH2_H
#define MESH2_H

typedef struct mesh2_bone_keyframe {
    vec3 translation;
    quat rotation;
    vec3 scaling;
    //mat44 get_local_transform() const { return compose44(translation, rotation, scaling); }
} mesh2_bone_keyframe;

typedef struct mesh2_bone {
    string name;
    int *parent_index;
    mesh2_bone_keyframe initial_pose;
    mat44 model_to_bone_matrix;
} mesh2_bone;

#pragma pack(1)
typedef struct mesh2_vertex {
    vec3 position, normal;
    vec2 texcoord;
    vec3 tangent, bitangent; // Gradient of texcoord.x and texcoord.y relative to position
    vec3 color;
    uint4 bone_indices;
    vec4 bone_weights;
} mesh2_vertex;
#pragma pack()

typedef struct mesh2_keyframe {
    int64_t key;
    array(mesh2_bone_keyframe) local_transforms;
} mesh2_keyframe;

typedef struct mesh2_animation {
    string name;
    array(mesh2_keyframe) keyframes;
} mesh2_animation;

typedef struct mesh2_material {
    string name;
    int first_triangle, num_triangles;
} mesh2_material;

// Value type which holds mesh information

typedef struct mesh2 {
    array(mesh2_vertex) vertices;
    array(uint3) triangles;
    array(mesh2_bone) bones;
    array(mesh2_animation) animations;
    array(mesh2_material) materials;
/*
    mat44 get_bone_pose(const array(mesh2_bone_keyframe) bone_keyframes, size_t index) {
        auto *b = &bones[index];
        return b->parent_index ? get_bone_pose(bone_keyframes, *b->parent_index) * bone_keyframes[index].get_local_transform() : bone_keyframes[index].get_local_transform();
    }

    mat44 get_bone_pose(size_t index) {
        auto *b = &bones[index];
        return b->parent_index ? get_bone_pose(*b->parent_index) * b->initial_pose.get_local_transform() : b->initial_pose.get_local_transform();
    }
*/
} mesh2;

API mesh2_bone_keyframe transform_keyframe(const mat44 t, const mesh2_bone_keyframe kf);
API mesh2_bone          transform_bone(const mat44 t, const mesh2_bone b);
API mesh2_vertex        transform_vertex(const mat44 t, const mesh2_vertex v);
API void                transform_mesh2(mesh2 *m, const mat44 t);
API mesh2*              mesh2_compute_tangents(mesh2 *m);
API mesh2               mesh2_load_obj(coord_system target_basis, const char *filename);
API array(mesh2)        mesh2_load_fbx(coord_system target, const char *filename);

API mesh                mesh2_as_mesh1(const mesh2 *m, const char *vertex_format);

/*
API mesh               mesh2_make_fullscreen_quad();
API mesh               mesh2_make_box(const vec3 & bmin, const vec3 & bmax);
API mesh               mesh2_apply_vertexcolor(mesh m, const vec3 & color);
API mesh               mesh2_invert_faces(mesh m);
API image              generate_single_color_image(const byte4 & color);
API image              load_image(const char * filename, bool is_linear);
*/

#if 0
// Support for rigid poses in three dimensions, defined by a rotation quaternion and a translation vector
struct pose { quat rotation {0,0,0,1}; vec3 position {0,0,0}; };
// pose mul    (const pose a, const pose b)  { return {a.rotation*b.rotation, a.position + qrot(a.rotation, b.position)}; }
// pose inverse(const pose p)                { auto q = conjugate(p.rotation); return {q, qrot(q,-p.position)}; }
// pose nlerp  (const pose a, const pose b, float t) { return {nlerp(a.rotation, b.rotation, t), lerp(a.position, b.position, t)}; }
// pose slerp  (const pose a, const pose b, float t) { return {slerp(a.rotation, b.rotation, t), lerp(a.position, b.position, t)}; }
// mat44 pose_matrix(const pose a) { return compose44(a.position, a.rotation, vec3(1,1,1)); }

vec3 transform_pose_vector  (pose p, vec3 vector)   { return transformq(p.rotation, vector); }
vec3 transform_pose_normal  (pose p, vec3 normal)   { return transform_pose_vector(p, normal); }
vec3 transform_pose_point   (pose p, vec3 point)    { return add3(p.position, transform_pose_vector(p, point)); }
vec3 transform_pose_tangent (pose p, vec3 tangent)  { return transform_pose_vector(p, tangent); }
quat transform_pose_quat    (pose p, quat q)        { vec3 v = transform_pose_vector(p, q.vec3); return quat(v.x,v.y,v.z,q.w); }
//mat44 transform_pose_matrix(pose p,  mat44 matrix)   { return transform_pose_matrix(pose_matrix(p), matrix); }
//vec3  transform_pose_scaling(pose p, vec3 scaling)   { return scaling44(pose_matrix(p), scaling); }
#endif

#endif

#ifdef MESH2_C
#pragma once

mesh2_bone_keyframe transform_keyframe(const mat44 t, const mesh2_bone_keyframe kf) {
    mesh2_bone_keyframe bkf = {transform_vector(t, kf.translation), transform_quat(t, kf.rotation), transform_scaling(t, kf.scaling)};
    return bkf;
}
mesh2_bone transform_bone(const mat44 t, const mesh2_bone b) { 
    mesh2_bone r = {b.name, b.parent_index, transform_keyframe(t,b.initial_pose)};
    transform_matrix(r.model_to_bone_matrix, t,b.model_to_bone_matrix);
    return r;
}
mesh2_vertex transform_vertex(const mat44 t, const mesh2_vertex v) { 
    mesh2_vertex mv = {transform_point(t,v.position), transform_normal(t,v.normal), v.texcoord, transform_tangent(t,v.tangent), transform_tangent(t,v.bitangent), v.color, v.bone_indices, v.bone_weights };
    return mv;
}
void transform_mesh2(mesh2 *m, const mat44 t) {
    array_foreach(m->vertices,mesh2_vertex,v) {
      *v = transform_vertex(t,*v);
    }
    array_foreach(m->bones,mesh2_bone,b) {
        *b = transform_bone(t,*b);
    }
    array_foreach(m->animations,mesh2_animation,a) {
        array_foreach(a->keyframes,mesh2_keyframe,k) {
            array_foreach(k->local_transforms,mesh2_bone_keyframe,lt) {
                *lt = transform_keyframe(t, *lt);
            }
        }
    }
}


mesh2* mesh2_compute_tangents(mesh2 *m) {

    // note: for shaders,
    // * vec3 bitangent can be reconstructed from single float with:
    //    normal = (world_matrix * vec4(normal, 0.0f)).xyz;
    //    tangent = (world_matrix * vec4(tangent, 0.0f)).xyz;
    //    bitangent = sign * cross(normal, tangent);
    // * and in that case, bitangent could be stored alone in [3] (.w) of vec4 tangent.

    array_foreach(m->vertices,mesh2_vertex,v) {
        v->tangent = v->bitangent = vec3(0,0,0);
    }
    array_foreach(m->triangles,uint3,t) {
        mesh2_vertex *v0 = &m->vertices[t->x], *v1 = &m->vertices[t->y], *v2 = &m->vertices[t->z];
        vec3 e1 = sub3(v1->position, v0->position), e2 = sub3(v2->position, v0->position);
        vec2 d1 = sub2(v1->texcoord, v0->texcoord), d2 = sub2(v2->texcoord, v0->texcoord);
        vec3 dpds = div3(vec3(d2.y*e1.x - d1.y*e2.x, d2.y*e1.y - d1.y*e2.y, d2.y*e1.z - d1.y*e2.z), cross2(d1, d2));
        vec3 dpdt = div3(vec3(d1.x*e2.x - d2.x*e1.x, d1.x*e2.y - d2.x*e1.y, d1.x*e2.z - d2.x*e1.z), cross2(d1, d2));
        v0->tangent = add3(v0->tangent, dpds); v1->tangent = add3(v1->tangent, dpds); v2->tangent = add3(v2->tangent, dpds);
        v0->bitangent = add3(v0->bitangent,dpdt); v1->bitangent = add3(v1->bitangent,dpdt); v2->bitangent = add3(v2->bitangent,dpdt);
    }
    array_foreach(m->vertices,mesh2_vertex,v) {
        v->tangent = norm3(v->tangent);
        v->bitangent = norm3(v->bitangent);
    }
    return m;
}

mesh2 mesh2_load_obj(coord_system target_basis, const char *filename) {
    mesh2 m = {0};
    array(vec3) vertices = 0;
    array(vec2) texcoords = 0;
    array(vec3) normals = 0;

    map_t(char*, uint32_t) vertex_map, *vmap = &vertex_map;
    map_create_keystr(vmap);

    char *data = file_readz(filename);
    array(char*) lines = string_split(data, "\r\n");

    for( int i = 0, c = array_count(lines); i < c; ++i) {
        char *line = lines[i];
        if( !line[0] ) continue; // if empty

        array(char*) tokens = string_split(line, "\t ");
        switch( tokens[0][0] ) {
            default:
            break; case '#':   // comment
            break; case 'u': { // usemtl
                char *name = tokens[1];
                if(!array_empty(m.materials)) array_back(m.materials)->num_triangles = array_count(m.triangles) - array_back(m.materials)->first_triangle;
                mesh2_material mm = {name, array_count(m.triangles), 0};
                array_push(m.materials, mm);
            }
            break; case 'f': { // face
                array(uint32_t) indices = 0;
                for( int i=1, e=array_count(tokens); i<e; ++i ) {
                    char *name = tokens[i];
                    uint32_t *found = map_find(vmap, name);
                    if( found ) {
                        array_push(indices, *found);
                    } else {
                        uint32_t index = (uint32_t)array_count(m.vertices);
                        map_insert(vmap, name, index);

                        bool no_texcoords = !!strstr(name, "//");
                        for(int i = 0; name[i]; ++i) if(name[i] == '/') name[i] = ' ';
                        int v = 0, vt = 0, vn = 0;
                        sscanf(name, "%d %d %d", &v, no_texcoords ? &vn : &vt, no_texcoords ? &vt : &vn);
                        mesh2_vertex vertex = {0};
                        if(v ) vertex.position = vertices[v-1];
                        if(vt) vertex.texcoord = texcoords[vt-1];
                        if(vn) vertex.normal = normals[vn-1];
                               vertex.color = vec3(1,1,1);
                        array_push(m.vertices, vertex);
                        array_push(indices, index);
                    }
                }
                for(int i=2,e=array_count(indices);i<e; ++i) {
                    uint3 triangle = {indices[0], indices[i-1], indices[i]};
                    array_push(m.triangles, triangle);
                }
                array_free(indices);
            }
            break; case 'v': // vertex
            /**/ if(tokens[0][1] == 't') {
                assert( array_count(tokens) >= 3 && "malformed vertex texcoord" );
                array_push( texcoords, vec2(string_tofloat(tokens[1]),/*1-*/string_tofloat(tokens[2])) );
            }
            else if(tokens[0][1] == 'n') {
                assert( array_count(tokens) >= 4 && "malformed vertex normal" );
                array_push( normals, vec3(string_tofloat(tokens[1]),string_tofloat(tokens[2]),string_tofloat(tokens[3])) );
            }
            else {
                assert( array_count(tokens) >= 4 && "malformed vertex" );
                array_push( vertices, vec3(string_tofloat(tokens[1]),string_tofloat(tokens[2]),string_tofloat(tokens[3])) );
            }
            break;
        }
        array_free(tokens);
    }
    array_free(lines);

    if( map_count(vmap) ) { // indices+verts .obj case
        array_free(vertices);
        array_free(texcoords);
        array_free(normals);

        map_destroy(vmap);
    } else { // verts-only .obj case
        array_resize(m.vertices, array_count(vertices));

        int i;
        i = 0; array_foreach(vertices,vec3,v) { m.vertices[i++].position = *v; }
        i = 0; array_foreach(texcoords,vec2,v) { m.vertices[i++].texcoord = *v; }
        i = 0; array_foreach(normals,vec3,v) { m.vertices[i++].normal = *v; }

        map_destroy(vmap);
    }

    if(!array_empty(m.materials)) {
        array_back(m.materials)->num_triangles = array_count(m.triangles) - array_back(m.materials)->first_triangle;
    }

    // rebase coord system

    coord_system source_basis = {axis_right, axis_up, axis_back};
    mat44 M; rebase44(M, source_basis, target_basis);
    transform_mesh2(&m, M);

    // 

    mesh2_compute_tangents(&m);
    return m;
}

mesh mesh2_as_mesh1(const mesh2 *m, const char *fmt) {
    mesh o;
    mesh_create(&o, fmt /*"p3 n3 t2 a3 i3" "w4u e4"*/, sizeof(m->vertices[0]),array_count(m->vertices),m->vertices, array_count(m->triangles)*3,m->triangles, 0);
    return o;
}

#endif
