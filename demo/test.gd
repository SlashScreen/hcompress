extends Node3D


func _ready() -> void:
	var img:Image = Image.create(1024, 1024, false, Image.FORMAT_RGB8)
	img.fill(Color.WHITE)
	var comp:Compressor = Compressor.new()
	print(comp.test_process_image(img).size() == img.get_width() * img.get_height())
