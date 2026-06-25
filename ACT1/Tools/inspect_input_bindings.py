import unreal


def asset(path: str):
    obj = unreal.load_asset(path)
    if not obj:
        unreal.log_error(f"Failed to load asset: {path}")
    return obj


bp = asset("/Game/Blueprint/BP_MainCharacter")
if bp:
    generated_class = bp.generated_class()
    cdo = unreal.get_default_object(generated_class)
    descend_action = cdo.get_editor_property("DescendAction")
    jump_action = cdo.get_editor_property("JumpAction")
    imc_air = cdo.get_editor_property("IMCAir")
    unreal.log(f"BP_MainCharacter DescendAction: {descend_action.get_path_name() if descend_action else 'None'}")
    unreal.log(f"BP_MainCharacter JumpAction: {jump_action.get_path_name() if jump_action else 'None'}")
    unreal.log(f"BP_MainCharacter IMCAir: {imc_air.get_path_name() if imc_air else 'None'}")

imc_air = asset("/Game/Input/Mappings/IMC_Air")
if imc_air:
    mappings = imc_air.get_editor_property("mappings")
    default_key_mappings = imc_air.get_editor_property("default_key_mappings")
    unreal.log(f"IMC_Air mapping count: {len(mappings)}")
    unreal.log(f"IMC_Air default key mappings type: {type(default_key_mappings).__name__}")
    unreal.log(f"IMC_Air default key mappings repr: {default_key_mappings}")
    unreal.log(f"IMC_Air default key mappings dir: {dir(default_key_mappings)}")
    for mapping in mappings:
        action = mapping.get_editor_property("action")
        key = mapping.get_editor_property("key")
        triggers = mapping.get_editor_property("triggers")
        unreal.log(
            "IMC_Air mapping: "
            f"action={action.get_path_name() if action else 'None'} "
            f"key={key.get_display_name()} "
            f"triggers={[type(t).__name__ for t in triggers]}"
        )
