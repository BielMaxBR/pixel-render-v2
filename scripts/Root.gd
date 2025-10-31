extends Control

var file: FileDialog

var model = null
onready var camera = $"%Camera"
onready var arrows = $"%Arrows"
onready var block_mouse = $"%Block".pressed
var angle_count = 32

func _ready():
	$VBoxContainer/Top/Buttons/File.get_popup().connect("id_pressed", self, "_file_popup_pressed")

	$"%RX".value = $"%CameraRoot/XCamera".rotation.x
	$"%RY".value = $"%CameraRoot".rotation.y
	$"%RZ".value = 0
	
	$"%PX".value = 0
	$"%PY".value = 0
	$"%PZ".value = 0
	
	$"%zoom".value =camera.position.z

	
func _physics_process(delta):
	arrows.points = []
	arrows.startThickness = $"%zoom".value/400
	arrows.endThickness = $"%zoom".value/400
	for i in range(0,angle_count):
		var step: float = 2*PI/angle_count
		var direction = Vector3.FORWARD.rotated(Vector3.UP,step * i)
		arrows.points.append(Vector3.ZERO)
		arrows.points.append(direction * ($"%zoom".value/2))
#
#	var texture: Texture = $Viewport/Viewport.get_texture()
#	var image := texture.get_data()
#	image.resize(64,64,Image.INTERPOLATE_NEAREST)
#	texture = ImageTexture.new()
#	texture.create_from_image(image)
#	$PopupPreview/TextureRect.texture = texture
	
	
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
	$"%Object".add_child(model)
	model.name = "model"
	_cancelled()


func _on_Viewport_gui_input(event: InputEvent):
	if block_mouse: return
	
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_WHEEL_DOWN:
			camera.position.z *= 1.1
			$"%zoom".value = camera.position.z
		if event.button_index == BUTTON_WHEEL_UP:
			camera.position.z *= 0.9
			$"%zoom".value = camera.position.z
	if event is InputEventMouseMotion:
		if Input.is_action_pressed("meio"):
			var velocity = (event as InputEventMouseMotion).relative
			$"%CameraRoot".rotate_y(-velocity.x/100)
			#$"%CameraRoot/XCamera".rotate_x(-velocity.y/100)
			$"%RX".value += -velocity.y/100
			#$"%CameraRoot/XCamera".rotation.x = clamp($"%CameraRoot/XCamera".rotation.x,PI,2*PI)
			$"%RX".value = $"%CameraRoot/XCamera".rotation.x
			$"%RY".value = $"%CameraRoot".rotation.y
			$"%RZ".value = camera.rotation.z

func _on_zoom_value_changed(value):
	camera.position.z = value


func _on_RX_value_changed(value):
	$"%CameraRoot/XCamera".rotation.x = value

func _on_RY_value_changed(value):
	$"%CameraRoot".rotation.y = value

func _on_RZ_value_changed(value):
	camera.rotation.z = value


func _on_PX_value_changed(value):
	$"%CameraRoot".position.x = value

func _on_PY_value_changed(value):
	$"%CameraRoot".position.y = value

func _on_PZ_value_changed(value):
	$"%CameraRoot".position.z = value


func _on_Block_toggled(button_pressed):
	block_mouse = button_pressed


