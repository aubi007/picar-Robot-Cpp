import numpy as np
import cv2

def slide_windows(img, treshold = 50, size = 8):
    height = img.shape[0]
    width = img.shape[1]   
    min_pix = (size * size * 100) / treshold

    print ('width x height ', width, ' x ', height, ' minpix ', min_pix )
    '''
    for x in range(1, width):
        val = img[height - 1, x]
        if val == 255:
            print 'x {}, y {}, val {}'.format(x, height-1, val)
    return
'''
    # slide windows
    for frame_x in range(0, width/size):              
        for frame_y in range(0, height/size):
            # count pixels in window
            pix = 0
            for x in range(size):
                for y in range(size):
#                    print 'x:y ', frame_x + x, frame_y + y
                    val = img[frame_x + x, frame_y + y]
                    print (val)
                    if val == 255: print (val)
                    if val == 255: pix = pix + 1
            # if the window has more pixels than treshold, add it to the result

            if pix >= min_pix:
                print ('win {},{}: {}').format(frame_x, frame_y, pix)

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
    if len(img.shape) > 2: color_depth = img.shape[2]
    else: color_depth = 1
    
    line_img = np.zeros(
        (
            img.shape[0],
            img.shape[1],
            color_depth
        ),
        dtype=np.uint8
    )
    
    img = np.copy(img)
    
    if lines is None:
        return
    
    for line in lines:
        for x1, y1, x2, y2 in line:
            cv2.line(line_img, (x1, y1), (x2, y2), color, thickness)
            
    img = cv2.addWeighted(img, 0.8, line_img, 1.0, 0.0)
    return img

def pipeline(image):
#    bgr_image = cv2.medianBlur(image, 3)
    
    
    # Convert input image to HSV
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Threshold the HSV image, keep only the red pixels
    lower_red_hue_range = cv2.inRange(hsv_image, (0, 100, 100), (10, 255, 255))
    upper_red_hue_range = cv2.inRange(hsv_image, (160, 100, 100), (179, 255, 255))
    # Combine the above two images
    red_hue_image = cv2.addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0)

#    return red_hue_image

    cropped_image = region_of_interest(red_hue_image)

    slide_windows(cropped_image)
    return cropped_image

    cannyed_image = cv2.Canny(cropped_image, 100, 200)

    lines = cv2.HoughLinesP(
        cannyed_image,
        rho=6,
        theta=np.pi / 180,
        threshold=160,
        lines=np.array([]),
        minLineLength=20,
        maxLineGap=25
    )

    line_image = draw_lines(
        cannyed_image,
        lines,
        thickness=5,
    )
    
    return line_image