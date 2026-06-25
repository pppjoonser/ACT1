import unreal


def log(value):
    unreal.log(str(value))


log("HAS AnimBlueprintFactory: {}".format(hasattr(unreal, "AnimBlueprintFactory")))
log("HAS BlendSpaceFactory1D: {}".format(hasattr(unreal, "BlendSpaceFactory1D")))
log("HAS BlendSpaceFactoryNew: {}".format(hasattr(unreal, "BlendSpaceFactoryNew")))
log("HAS BlendSpace1DFactoryNew: {}".format(hasattr(unreal, "BlendSpace1DFactoryNew")))
log("HAS ACT1CharacterAnimInstance: {}".format(hasattr(unreal, "ACT1CharacterAnimInstance")))

mesh = unreal.load_asset("/Game/Characters/Heroes/Mannequin/Meshes/SKM_Manny")
log("MESH: {}".format(mesh))
if mesh:
    skeleton = mesh.skeleton
    log("SKELETON: {}".format(skeleton.get_path_name()))
