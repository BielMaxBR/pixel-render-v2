extends Node2D

export var obj_packed: PackedScene
export var size: Vector2
export var anim_name: String = "run"
onready var viewport = $"ViewportContainer/Viewport"
onready var scene = viewport.get_node("3D")

var obj: Spatial
var player: AnimationPlayer
var anim: Animation
var fps = 24

var total_frames = 0
var total = 0
var angles = 8

func _ready():
	var inst = obj_packed.instance()
	scene.add_child(inst)
	obj = inst
	player = obj.get_node("AnimationPlayer")
	yield(get_tree().create_timer(0.5),"timeout") 

	anim = player.get_animation(anim_name)
	player.assigned_animation = anim_name
	anim.loop = true
	total_frames = anim.length*fps
	total = anim.length/total_frames
	
	var big_sheet = Image.new()
	
	big_sheet.create(size.x*total_frames,size.y*angles,false,Image.FORMAT_RGBA8)
	
	var big_list = []
	for i in range(0,angles):
		var img = render(i)
		while img is GDScriptFunctionState: 
			img = yield(img, "completed")
			# print(img)
#			img = img.resume(i)
		
		big_list.append(img)
		yield(get_tree().create_timer(0.5),"timeout") 
		obj.rotate_y(deg2rad(360/float(-angles)))
	
	for i in range(0,angles):
		var image: Image = big_list[i]
		var rect: Rect2 = Rect2(Vector2(0,0),image.get_size())
		big_sheet.blit_rect(image, rect, Vector2(0, size.y*i))
	
	big_sheet.save_png("res://export/" + obj.name + "_" + anim_name +"_sheet.png")

func render(num) -> Image:
	var list = []
#	for current in range(total_frames-1,-1,-1):
	for current in range(0,total_frames):
		yield(get_tree(),"idle_frame")
		player.advance(total)

#		print(total* (current),"--", anim.length)
		var image = viewport.get_texture().get_data()
#		image.save_png("res://export/" + obj.name + "_" + str(current)+".png")
		list.push_front(image)

	var sheet = Image.new()
	
	sheet.create(size.x*total_frames,size.y,false,list[0].get_format())
	
	for i in range(0,total_frames):
		var image: Image = list[i]
		var center: Vector2 = image.get_size()/2
		var rect: Rect2 = Rect2(center-(size/2),center+(size/2))
		sheet.blit_rect(image, rect, Vector2(size.x*i, 0))
	print(sheet)
	return sheet
#	sheet.save_png("res://export/" + obj.name + "_" + str(num) + "_sheet.png")
#	get_tree().quit()
