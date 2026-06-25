import unreal


ASSETS = [
    "/Game/Characters/Heroes/Mannequin/Animations/AnimModifiers/FootstepEffectTagModifier",
    "/Game/Effects/Blueprints/B_WeaponImpacts",
    "/Game/Weapons/B_Weapon",
    "/Game/Audio/Blueprints/B_MusicManagerComponent_Base",
    "/Game/Audio/Blueprints/GeneralAudioFunctions",
]


registry = unreal.AssetRegistryHelpers.get_asset_registry()

for asset_path in ASSETS:
    package_name = asset_path
    referencers = registry.get_referencers(package_name, unreal.AssetRegistryDependencyOptions(True, True, True, True))
    unreal.log("REFERENCERS {}".format(asset_path))
    if referencers:
        for ref in sorted(set(str(r) for r in referencers)):
            unreal.log("  {}".format(ref))
    else:
        unreal.log("  <none>")
