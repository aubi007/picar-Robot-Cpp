import numpy as np
import cv2

def region_of_interest(img):
    # Define a blank matrix that matches the image height/width.
    mask = np.zeros_like(img)
   
    # get the image shape
    height = img.shape[0]
    width = img.shape[1]
    if len(img.shape) > 2: color = img.shape[2]
    else: color = 1

    # define the polygon vertices 
    vertices = [
        (0, height),
        (width / 2, height / 2),
        (width, height),
    ]
    
    # Create a match color with the same color channel counts.
    match_mask_color = (255,) * color
      
    # Fill inside the polygon
    cv2.fillPoly(mask, np.array([vertices], dtype=np.int32), match_mask_color)
    
    # Returning the image only where mask pixels match
    masked_image = cv2.bitwise_and(img, mask)
    return masked_image

def draw_lines(img, lines, color=[255, 0, 0], thickness=3):
    # If there are no lines to draw, exit.
    if lines is None:
        return
    
    # Make a copy of the original image.
    img = np.copy(img)
    
    # Create a blank image that matches the original in size.
    line_img = np.zeros(
        (
            img.shape[0],
            img.shape[1],
            3
        ),
        dtype=np.uint8,
    )
    print ('filter1 lines: ', lines)
        
    # Loop over all lines and draw them on the blank image.
    for line in lines:
        print ('filter1 line: ', line)
        for x1, y1, x2, y2 in line:
            cv2.line(line_img, (x1, y1), (x2, y2), color, thickness)

    # Merge the image with the lines onto the original.
    img = cv2.addWeighted(img, 0.8, line_img, 1.0, 0.0)

    # Return the modified image.
    return img

def pipeline(img):
    
#    cropped_image = region_of_interest(img)

    # Convert to grayscale here.
#    gray_image = cv2.cvtColor(cropped_image, cv2.COLOR_RGB2GRAY)
    gray_image = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
            
    # Call Canny Edge Detection here.
    cannyed_image = cv2.Canny(gray_image, 100, 200)
    
    # Moved the cropping operation to the end of the pipeline.
    cropped_image = region_of_interest(cannyed_image)
    
    lines = cv2.HoughLinesP(
#        cannyed_image,
        cropped_image,
        rho=6,
        theta=np.pi / 60,
        threshold=160,
        lines=np.array([]),
        minLineLength=40,
        maxLineGap=25
    )
    
#    print lines
    line_image = draw_lines(img, lines)
    
    return line_image