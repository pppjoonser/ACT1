import unreal


def log(value):
    unreal.log(str(value))


log("HAS BlueprintEditorLibrary: {}".format(hasattr(unreal, "BlueprintEditorLibrary")))
if hasattr(unreal, "BlueprintEditorLibrary"):
    log("HAS reparent_blueprint: {}".format(hasattr(unreal.BlueprintEditorLibrary, "reparent_blueprint")))
    log("HAS compile_blueprint: {}".format(hasattr(unreal.BlueprintEditorLibrary, "compile_blueprint")))
    log("HAS refresh_open_editors_for_blueprint: {}".format(hasattr(unreal.BlueprintEditorLibrary, "refresh_open_editors_for_blueprint")))

log("HAS AnimationBlueprintLibrary: {}".format(hasattr(unreal, "AnimationBlueprintLibrary")))
