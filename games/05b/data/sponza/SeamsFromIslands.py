import bpy
prev_active_obj = bpy.context.scene.objects.active
for o in bpy.context.selected_objects:
    if o.type == "MESH":
        bpy.context.scene.objects.active = o
        bpy.ops.object.mode_set(mode="EDIT")
        bpy.ops.mesh.select_all(action="SELECT")
        bpy.ops.uv.seams_from_islands()
        bpy.ops.mesh.select_all(action="DESELECT")
        bpy.ops.object.mode_set(mode="OBJECT")
bpy.context.scene.objects.active = prev_active_obj
