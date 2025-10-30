extends Node2D


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
export var direction_count = 8
export var height = 80
export var width = 80

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.
	$AnimationPlayer.play("run")
	$Sprite.hframes = $Sprite.texture.get_width() / width
	$Sprite.region_rect.size = Vector2($Sprite.texture.get_width(), height)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	var mouse = get_global_mouse_position()
	var angle = $Sprite.global_position.direction_to(mouse).rotated(deg2rad(-90)).angle()
	set_angle(angle)

func set_angle(angle):
	var _angle = round(rad2deg(angle))+180
	var step = 360 / direction_count 
	var direction = round(_angle / step)
	$Sprite.region_rect.position.y = direction * height
