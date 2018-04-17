import pygame

pygame.init()
#pygame.display.init()
screen = pygame.display.set_mode( ( 720, 480 ), pygame.FULLSCREEN )
clock = pygame.time.Clock()
done = False


available = pygame.font.get_fonts()
print(available)

#font = pygame.font.Font(None, 100)
font = pygame.font.SysFont("robotocondensed", 350)
font.set_bold(False)

text = font.render("{:2}:{:2}".format(1,20), True, (255,255,255))


while not done:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True
        if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
            done = True
    
    screen.fill((0,0,0))
    screen.blit(text, (360 - text.get_width() // 2, 240 - text.get_height() // 2))
    
    pygame.display.flip()
    
    clock.tick(10)

