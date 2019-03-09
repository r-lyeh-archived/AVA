import os
import bpy
import bmesh
import struct
import datetime

EXPORT_DIRECTORY         = os.path.join(os.path.expanduser("~"), "Desktop", "BlenderMeshHeaders", datetime.datetime.today().strftime("%d_%b_%Y_%Hh_%Mm_%Ss"))
EXPORT_LEFT_HANDED       = True
EXPORT_COUNTER_CLOCKWISE = True

selected_objects = bpy.context.selected_objects
sort_keys = dict(zip(selected_objects, [o.name for o in selected_objects]))
selected_objects.sort(key=sort_keys.get)

def UVFromVertex(uv_layer, v):
    for l in v.link_loops:
        return l[uv_layer].uv
    return Vector((0.0, 0.0))

if len(selected_objects) > 0:
    if not os.path.exists(EXPORT_DIRECTORY):
        os.makedirs(EXPORT_DIRECTORY)
    
    enums_header_file         = open(os.path.join(EXPORT_DIRECTORY, "mesh_enums.h"), "w")
    verts_header_file         = open(os.path.join(EXPORT_DIRECTORY, "mesh_vertices.h"), "w")
    uvs_header_file           = open(os.path.join(EXPORT_DIRECTORY, "mesh_uvs.h"), "w")
    normals_header_file       = open(os.path.join(EXPORT_DIRECTORY, "mesh_normals.h"), "w")
    texture_names_header_file = open(os.path.join(EXPORT_DIRECTORY, "mesh_texture_names.h"), "w")
    texture_ids_header_file   = open(os.path.join(EXPORT_DIRECTORY, "mesh_texture_ids.h"), "w")
    submesh_vert_begin_file   = open(os.path.join(EXPORT_DIRECTORY, "submesh_vertex_begin.h"), "w")
    submesh_vert_end_file     = open(os.path.join(EXPORT_DIRECTORY, "submesh_vertex_end.h"), "w")

    submesh_vert_counter = 0

    texture_names = []

    for obj in selected_objects:
        if obj.type == "MESH":
            bm = bmesh.new()
            bm.from_mesh(obj.data)
            bmesh.ops.triangulate(bm, faces=bm.faces, quad_method=0, ngon_method=0)
            split_edges = []
            for e in bm.edges:
                if e.seam:
                    split_edges.append(e)
            bmesh.ops.split_edges(bm, edges=split_edges)
            
            enums_header_file.write('%s,\n' % (obj.name.upper()))
            
            verts_x = []
            verts_y = []
            verts_z = []
            
            verts_normals_x = []
            verts_normals_y = []
            verts_normals_z = []
            
            for v in bm.verts:
                verts_x.append(v.co.x)
                verts_y.append(v.co.z)
                verts_z.append(v.co.y if EXPORT_LEFT_HANDED else -v.co.y)

                verts_normals_x.append(v.normal.x)
                verts_normals_y.append(v.normal.z)
                verts_normals_z.append(v.normal.y if EXPORT_LEFT_HANDED else -v.normal.y)

            submesh_vert_begin_file.write('%i,\n' % (submesh_vert_counter))

            for f in bm.faces:
                verts_header_file.write('%f, %f, %f,\n%f, %f, %f,\n%f, %f, %f,\n' % (
                                        verts_x[f.verts[0].index],
                                        verts_y[f.verts[0].index],
                                        verts_z[f.verts[0].index],
                                        
                                        verts_x[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                        verts_y[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                        verts_z[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                        
                                        verts_x[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index],
                                        verts_y[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index],
                                        verts_z[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index]))
                submesh_vert_counter += 3
            
            submesh_vert_end_file.write('%i,\n' % (submesh_vert_counter))
            
            active_uv_layer = bm.loops.layers.uv.active
            
            uvs_x = []
            uvs_y = []
            
            for v in bm.verts:
                uv = UVFromVertex(active_uv_layer, v)
                uvs_x.append(uv.x)
                uvs_y.append(uv.y)
            
            for f in bm.faces:
                uvs_header_file.write('%f, %f,\n%f, %f,\n%f, %f,\n' % (
                                      uvs_x[f.verts[0].index],
                                      uvs_y[f.verts[0].index],
                                      
                                      uvs_x[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                      uvs_y[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                      
                                      uvs_x[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index],
                                      uvs_y[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index]))
            
            for f in bm.faces:
                normals_header_file.write('%f, %f, %f,\n%f, %f, %f,\n%f, %f, %f,\n' % (
                                          verts_normals_x[f.verts[0].index],
                                          verts_normals_y[f.verts[0].index],
                                          verts_normals_z[f.verts[0].index],
                                          
                                          verts_normals_x[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                          verts_normals_y[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                          verts_normals_z[f.verts[1 if EXPORT_COUNTER_CLOCKWISE else 2].index],
                                          
                                          verts_normals_x[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index],
                                          verts_normals_y[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index],
                                          verts_normals_z[f.verts[2 if EXPORT_COUNTER_CLOCKWISE else 1].index]))
            
            texture = obj.data.uv_textures.active.data[0].image
            if texture:
                texture_names.append(texture.name)
            else:
                texture_names.append('')

    unique_texture_names = list(set(texture_names))
    unique_texture_names.remove('')

    texture_names_header_file.write('0,\n')
    for texture_name in unique_texture_names:
        texture_names_header_file.write('"%s",\n' % (texture_name))

    for texture in texture_names:
        if texture == '':
            texture_ids_header_file.write('0,\n')
        else:
            for i, unique_texture_name in enumerate(unique_texture_names):
                if texture == unique_texture_name:
                    texture_ids_header_file.write('%d,\n' % (i + 1))

    enums_header_file.close()
    verts_header_file.close()
    uvs_header_file.close()
    normals_header_file.close()
    texture_names_header_file.close()
    texture_ids_header_file.close()
    submesh_vert_begin_file.close()
    submesh_vert_end_file.close()
