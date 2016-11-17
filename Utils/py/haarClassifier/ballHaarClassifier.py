import cv2

ballCascade = cv2.CascadeClassifier('cascade1.xml')


#Load a color image
img = cv2.imread('RoHOW2015G1A_RoHOW2015_00065315_U.png')
#convert it to grayscale
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

balls = ballCascade.detectMultiScale(
    gray,
    scaleFactor=1.1,
    minNeighbors=202, # needs adjustment
    minSize=(10, 10),
    maxSize=(40, 40)
)

# Draw a rectangle around the balls
for (x, y, w, h) in balls:
    cv2.rectangle(gray, (x, y), (x+w, y+h), (0, 255, 0), 2)

print balls
# Display the resulting img
cv2.imshow('BallDetection', gray)
cv2.waitKey(0)
cv2.destroyAllWindows()