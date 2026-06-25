import unreal


ASSET_ROOT = "/Game/Input/Actions"
DESCEND_ACTION_PATH = f"{ASSET_ROOT}/IA_Descend"
BLUEPRINT_PATH = "/Game/Blueprint/BP_MainCharacter"
IMC_AIR_PATH = "/Game/Input/Mappings/IMC_Air"


def load_asset(path: str):
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        unreal.log_error(f"Failed to load asset: {path}")
    return asset


def ensure_descend_action():
    existing = unreal.EditorAssetLibrary.load_asset(DESCEND_ACTION_PATH)
    if existing:
        unreal.log(f"IA_Descend already exists: {existing.get_path_name()}")
        return existing

    source = unreal.EditorAssetLibrary.load_asset("/Game/Input/Actions/IA_Crouch")
    if not source:
        unreal.log_error("Failed to load source IA_Crouch for duplication")
        return None

    action = unreal.EditorAssetLibrary.duplicate_asset("/Game/Input/Actions/IA_Crouch", DESCEND_ACTION_PATH)
    action = unreal.EditorAssetLibrary.load_asset(DESCEND_ACTION_PATH)
    if not action:
        unreal.log_error("Failed to duplicate IA_Crouch to IA_Descend")
        return None

    action.set_editor_property("value_type", unreal.InputActionValueType.BOOLEAN)
    action.set_editor_property("triggers", [])
    unreal.EditorAssetLibrary.save_loaded_asset(action)
    unreal.log(f"Created IA_Descend: {action.get_path_name()}")
    return action


def update_main_character_blueprint(descend_action):
    bp = load_asset(BLUEPRINT_PATH)
    if not bp:
        return

    generated_class = bp.generated_class()
    cdo = unreal.get_default_object(generated_class)
    cdo.set_editor_property("DescendAction", descend_action)
    unreal.EditorAssetLibrary.save_loaded_asset(bp)
    unreal.log(f"Updated BP_MainCharacter DescendAction -> {descend_action.get_path_name()}")


def add_air_mapping(descend_action):
    imc_air = load_asset(IMC_AIR_PATH)
    if not imc_air:
        return

    subsystem = unreal.get_editor_subsystem(unreal.EnhancedInputEditorSubsystem)
    if not subsystem:
        unreal.log_error("Failed to get EnhancedInputEditorSubsystem")
        return

    unreal.log(f"EnhancedInputEditorSubsystem dir: {dir(subsystem)}")


descend_action = ensure_descend_action()
if descend_action:
    update_main_character_blueprint(descend_action)
    add_air_mapping(descend_action)
