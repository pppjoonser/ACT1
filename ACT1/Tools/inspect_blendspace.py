import unreal


asset = unreal.load_asset("/Game/Animation/BS_CharacterMove")
unreal.log("ASSET: {}".format(asset))

if asset:
    for name in ("axis_to_scale_animation", "interpolation_param", "sample_interpolation", "target_weight_interpolation_speed_per_sec"):
        if hasattr(asset, name):
            unreal.log("{}: {}".format(name, getattr(asset, name)))

    for axis_name in ("horizontal_axis", "vertical_axis"):
        if hasattr(asset, axis_name):
            axis = getattr(asset, axis_name)
            unreal.log(
                "{} => name={} min={} max={}".format(
                    axis_name,
                    getattr(axis, "display_name", None),
                    getattr(axis, "min", None),
                    getattr(axis, "max", None),
                )
            )
