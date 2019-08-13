# Action Selection Python Project

The scripts were developed for Benjamin Schlotters Bachelor thesis:
"Selection of Actions based on Forward Simulations" and the following Papers:
[Simulation based selection of actions for a humanoid soccer-robot](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/2016-mellmannschlotterblum.pdf)

[Advances on simulation based selection of actions for a humanoid soccer-robot](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/hsr-mellmannschlotter-17.pdf)


The following scripts are meant to be executed:
`run_simulation.py`  
Visualizes the basic simulation behavior for three kicks.  

`run_simulation_with_particleFilter`  

`simulate_every_pos`  

`play_striker`  
Visualizes the path to goal from one position on the field for one strategy. Our notion of a strategy is explained in the [Humanoids Paper](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/hsr-mellmannschlotter-17.pdf)  

`compare_decision_schemes_newsim.py`  
The path to the goal is calculated for each strategy systematically for all field positions with a fixed step size. The simulated actions are logged to a pickle file.  

`compare_decision_schemes_newsim_random.py`  
For a certain amount of random positions on the field the path to the goal is calculated for each strategy and the simulated actions are logged to a pickle file.  


## Todo
- das generierte Potentialfeld muss wieder nutzbar gemacht werden  
- zusammenführen von compare_decision_schemes_newsim.py und compare_decision_schemes_newsim_random.py  
- particle filter für den roboter ordentlich implementieren  
