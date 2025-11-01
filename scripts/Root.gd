extends Control

var file: FileDialog

var model = null
onready var camera = $"%Camera"
onready var arrows = $"%Arrows"
onready var block_mouse = $"%Block".pressed
var angle_count = 8

var sprite_size = Vector2(64,64)

var arrows_tween: SceneTreeTween

func _ready():
	$VBoxContainer/Top/Buttons/File.get_popup().connect("id_pressed", self, "_file_popup_pressed")

	$"%RX".value = $"%CameraRoot/XCamera".rotation_degrees.x
	$"%RY".value = $"%CameraRoot".rotation_degrees.y
	$"%RZ".value = 0
	
	$"%PX".value = 0
	$"%PY".value = 0
	$"%PZ".value = 0
	
	$"%zoom".value = camera.position.z

	$"%directions".value = angle_count
	$"%Arrows".hide()
	arrows_tween.kill() # a porra do spinvalue ativa sinal via código
func _physics_process(delta):
	arrows.points = []
	arrows.startThickness = $"%zoom".value/400
	arrows.endThickness = $"%zoom".value/400
	for i in range(0,angle_count):
		var step: float = 2*PI/angle_count
		var direction = Vector3.FORWARD.rotated(Vector3.UP,step * i)
		arrows.points.append(Vector3.ZERO)
		arrows.points.append(direction * ($"%zoom".value/2))
	
	$ResultContainer/Viewport/Camera2D.zoom = Vector2.ONE * (1200/max(sprite_size.x, sprite_size.y))
	$ResultContainer/Viewport.size = sprite_size
	#var texture: Texture = $Viewport/Viewport.get_texture()
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
			$"%CameraRoot/XCamera".rotate_x(-velocity.y/100)
			#$"%RX".value += -velocity.y/100
			#$"%CameraRoot/XCamera".rotation.x = clamp($"%CameraRoot/XCamera".rotation.x,PI,2*PI)
			$"%RX".value = $"%CameraRoot/XCamera".rotation_degrees.x
			$"%RY".value = $"%CameraRoot".rotation_degrees.y
			$"%RZ".value = camera.rotation_degrees.z

func _on_zoom_value_changed(value):
	camera.position.z = value
	camera.size = camera.position.z

func _on_RX_value_changed(value):
	$"%CameraRoot/XCamera".rotation_degrees.x = value

func _on_RY_value_changed(value):
	$"%CameraRoot".rotation_degrees.y = value

func _on_RZ_value_changed(value):
	camera.rotation_degrees.z = value


func _on_PX_value_changed(value):
	$"%CameraRoot".position.x = value

func _on_PY_value_changed(value):
	$"%CameraRoot".position.y = value

func _on_PZ_value_changed(value):
	$"%CameraRoot".position.z = value


func _on_Block_toggled(button_pressed):
	block_mouse = button_pressed


func _on_type_item_selected(index):
	match index:
		0: #perspective
			camera.projection = Camera.PROJECTION_PERSPECTIVE
		1: #orthogonal
			camera.projection = Camera.PROJECTION_ORTHOGONAL


func _on_directions_value_changed(value):
	angle_count = value
	$"%Arrows".show()
	if arrows_tween != null:
		arrows_tween.kill()
		arrows_tween = null
	arrows_tween = get_tree().create_tween()
	arrows_tween.tween_callback($"%Arrows", "hide").set_delay(3)


func _on_AnimationButton_toggled(button_pressed):
	$"%AnimationMenu".visible = button_pressed
