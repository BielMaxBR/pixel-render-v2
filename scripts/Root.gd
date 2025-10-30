extends Control

var file: FileDialog

var model = null
onready var object = $VBoxContainer/HSplitContainer/ViewportPanel/Viewport/Viewport/Object

func _ready():
	$VBoxContainer/Top/Buttons/File.get_popup().connect("id_pressed", self, "_file_popup_pressed")

func _file_popup_pressed(id: int):
	match id:
		0: # New
			file = FileDialog.new()
			file.popup_exclusive = true
			file.access = FileDialog.ACCESS_FILESYSTEM
			file.mode = FileDialog.MODE_OPEN_FILE
			file.rect_min_size = Vector2(600,500)
			file.get_cancel().connect("pressed", self, "_cancelled")
			file.get_close_button().connect("pressed", self, "_cancelled")
			file.connect("file_selected", self, "_new_file")
			file.set_filters(PoolStringArray(["*.gltf ; GLTF Files"]))
			
			add_child(file)
			file.popup()

func _cancelled():
	if not file == null:
		file.queue_free()
		file = null

func _new_file(path: String):
	print("abrindo",path)
	if model != null:
		model.queue_free()
		model = null
	
	var model_file:PackedScene = load(path)
	model = model_file.instance()
	object.add_child(model)
	model.name = "model"
	_cancelled()
