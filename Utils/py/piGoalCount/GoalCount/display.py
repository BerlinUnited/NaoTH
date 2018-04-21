import pygame


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
    self.update()

  def setGameTime(self, t):
    self.gameTime = t
    self.update()
    
  def set_score(self, a, b):
    self.goalsLeft = a
    self.goalsRight = b
    self.update()
    
  def update(self):
    color_bg = (255,255,255)
    color_text = (0,0,0)
    
    offset_goals_y = 70
    offset_time_y = -30
    
    self.screen.fill(color_bg)
    
    textGoals = self.fontScore.render("{}:{}".format(self.goalsLeft,self.goalsRight), True, color_text)
    self.screen.blit(textGoals, (self.width // 2 - textGoals.get_width() // 2, self.height // 2 - textGoals.get_height() // 2 + offset_goals_y))
    
    mins = self.gameTime // (1000*60)
    secs = (self.gameTime - mins*(1000*60)) // 1000
    textTime = self.fontTime.render("{}m {}s".format(mins, secs), True, color_text)
    self.screen.blit(textTime, (self.width // 2 - textTime.get_width() // 2, offset_time_y))
    
    pygame.display.flip()
    
  def end(self):
    pygame.quit()
