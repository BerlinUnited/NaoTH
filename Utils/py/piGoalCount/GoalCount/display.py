import pygame


class Display:
  def __init__(self, a = 0, b = 0):
    pygame.init()
    # hite the mouse
    pygame.mouse.set_visible(False)
    
    self.width = 800
    self.height = 480
    self.screen = pygame.display.set_mode( ( self.width, self.height ), pygame.FULLSCREEN )

    self.font = pygame.font.SysFont("robotocondensed", 350)
    self.font.set_bold(False)

    self.set_score(a, b)
  

  def set_score(self, a, b):
    print("set_score")
    text = self.font.render("{:2}:{:2}".format(a,b), True, (255,255,255))
    
    self.screen.fill((0,0,0))
    self.screen.blit(text, (self.width // 2 - text.get_width() // 2, self.height // 2 - text.get_height() // 2))
    
    pygame.display.flip()
