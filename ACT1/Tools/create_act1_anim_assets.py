import unreal


TARGET_DIR = "/Game/ACT1/Animation"
MANNY_MESH_PATH = "/Game/Characters/Heroes/Mannequin/Meshes/SKM_Manny"
SOURCE_BS_PATH = "/Game/Characters/Heroes/Mannequin/Animations/Locomotion/Unarmed/BS_MM_Unarmed_Jog_Walk"
TARGET_ABP_PATH = TARGET_DIR + "/ABP_ACT1_Manny"
TARGET_BS_PATH = TARGET_DIR + "/BS_ACT1_Ground"


def log(message):
    unreal.log(str(message))


def ensure_directory(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def duplicate_if_missing(source_path, target_path):
    if unreal.EditorAssetLibrary.does_asset_exist(target_path):
        log("Asset already exists: {}".format(target_path))
        return unreal.load_asset(target_path)

    result = unreal.EditorAssetLibrary.duplicate_asset(source_path, target_path)
    if not result:
        raise RuntimeError("Failed to duplicate {} to {}".format(source_path, target_path))

    log("Duplicated {} -> {}".format(source_path, target_path))
    return unreal.load_asset(target_path)


def create_blank_anim_blueprint(target_path, skeleton):
    if unreal.EditorAssetLibrary.does_asset_exist(target_path):
        log("AnimBlueprint already exists: {}".format(target_path))
        return unreal.load_asset(target_path)

    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    factory = unreal.AnimBlueprintFactory()
    factory.set_editor_property("target_skeleton", skeleton)
    factory.set_editor_property("parent_class", unreal.ACT1CharacterAnimInstance)

    package_path, asset_name = target_path.rsplit("/", 1)
    blueprint = asset_tools.create_asset(asset_name, package_path, unreal.AnimBlueprint, factory)
    if not blueprint:
        raise RuntimeError("Failed to create blank AnimBlueprint at {}".format(target_path))

    log("Created blank AnimBlueprint: {}".format(target_path))
    return blueprint


ensure_directory(TARGET_DIR)

mesh = unreal.load_asset(MANNY_MESH_PATH)
if not mesh:
    raise RuntimeError("Failed to load Manny mesh: {}".format(MANNY_MESH_PATH))

skeleton = mesh.skeleton
if not skeleton:
    raise RuntimeError("Manny mesh has no skeleton")

log("Using skeleton: {}".format(skeleton.get_path_name()))

blend_space = duplicate_if_missing(SOURCE_BS_PATH, TARGET_BS_PATH)
anim_blueprint = create_blank_anim_blueprint(TARGET_ABP_PATH, skeleton)
unreal.BlueprintEditorLibrary.compile_blueprint(anim_blueprint)
log("Compiled AnimBlueprint: {}".format(TARGET_ABP_PATH))

unreal.EditorAssetLibrary.save_loaded_asset(blend_space)
unreal.EditorAssetLibrary.save_loaded_asset(anim_blueprint)

log("DONE ABP: {}".format(TARGET_ABP_PATH))
log("DONE BS: {}".format(TARGET_BS_PATH))
