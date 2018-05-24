import pygame
from datetime import timedelta
from datetime import datetime
from time import sleep

class Display:
  def __init__(self, a = 0, b = 0):
    pygame.init()
    # hite the mouse
    pygame.mouse.set_visible(False)
    
    self.width = 800
    self.height = 480
    self.screen = pygame.display.set_mode( ( self.width, self.height ), pygame.FULLSCREEN )

    self.fontScore = pygame.font.SysFont("robotocondensed", 350)
    self.fontScore.set_bold(False)

    self.fontTime = pygame.font.SysFont("robotocondensed", 175)
    self.fontTime.set_bold(False)
    
    self.goalsLeft = a
    self.goalsRight = b
    self.gameTime = 0
    self.updateText()

  def setValues(self, a, b, t):
    self.goalsLeft = a
    self.goalsRight = b
    self.gameTime = t
    self.updateText()  
    
  def clear(self):
    color_bg = (255,255,255)
    self.screen.fill(color_bg)
    
  def updateText(self):
    self.clear()
    
    color_text = (0,0,0)
    
    offset_goals_y = 70
    offset_time_y = -30
    
    textGoals = self.fontScore.render("{}:{}".format(self.goalsLeft,self.goalsRight), True, color_text)
    self.screen.blit(textGoals, (self.width // 2 - textGoals.get_width() // 2, self.height // 2 - textGoals.get_height() // 2 + offset_goals_y))
    
    #mins = self.gameTime // (1000*60)
    #secs = (self.gameTime - mins*(1000*60)) // 1000
    
    d = datetime(2010, 1, 1, 0, 0, 0) + timedelta(milliseconds=self.gameTime)
    textTime = self.fontTime.render('{:%M:%S}'.format(d), True, color_text)
    self.screen.blit(textTime, (self.width // 2 - textTime.get_width() // 2, offset_time_y))
    
    pygame.display.flip()
    
    
  def updateImage(self, i):
    self.clear()
    
    imgs = [
      pygame.image.load("./img/bdr-500.png").convert(),
      pygame.image.load("./img/bu-500.png").convert(),
    ]
    
    im = imgs[i]
    self.screen.blit(im, (self.width // 2 - im.get_width() // 2, self.height // 2 - im.get_height() // 2))
    pygame.display.flip()
    
    
  def end(self):
    pygame.quit()
    
    
if __name__ == '__main__':
  print ("test display")
  display = Display()
  
  i = 0
  while True:
    #display.setValues(2,7,142)
    display.updateImage(i)
    i += 1
    if i > 1:
      i = 0
    sleep(2)

  display.end()
