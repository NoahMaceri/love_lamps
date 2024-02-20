from PIL import Image
import argparse
import os

# Construct the argument parser and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--input", required=True, help="path to input gif")

args = vars(ap.parse_args())

# Load the input image
im = Image.open(args["input"])

# Print frame delay
print(f'Delay between frames: {im.info["duration"]}')

# if frames folder present, delete
if os.path.exists("frames"):
	# detele even if not empty
	os.system("rmdir /S /Q frames")

# Create folder
os.mkdir("frames")

# Iterate through every frame of the gif, resize to 64x64 and save
for frame in range(0, im.n_frames):
	im.seek(frame)
	im.convert('L').resize((32, 32)).save("frames/frame" + str(frame).zfill(3) + ".png")

# Close the image
im.close()
