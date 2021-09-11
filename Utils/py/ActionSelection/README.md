# Action Selection Python Project

The scripts were developed for for following publications:

* "Selection of Actions based on Forward Simulations", Bachelor thesis, Benjamin Schlotter 

* [Simulation based selection of actions for a humanoid soccer-robot](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/2016-mellmannschlotterblum.pdf)

* [Advances on simulation based selection of actions for a humanoid soccer-robot](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/hsr-mellmannschlotter-17.pdf)

## Experiments
The following scripts are meant to be executed:

### Basic Action Selection Scheme
Visualizes the basic simulation behavior for three kicks.  
```
python run_simulation.py
```

### Extended  Action Selection Scheme
Computes the best kick direction based on particle filter sampling over possible angles. 
```
python run_simulation_with_particleFilter.py
```

### Map Experiment?
Calculate the decision in a grid over the whole field and calculate a map visualizing the decisions.
```
python simulate_every_pos.py
```

### Path, Strategy
Visualizes the path to goal from one position on the field for one strategy. Our notion of a strategy is explained in the [Humanoids Paper](http://www.naoteamhumboldt.de/wp-content/papercite-data/pdf/hsr-mellmannschlotter-17.pdf)  
```
python play_striker.py
```

### Compare Decisions Schemes
The path to the goal is calculated for each strategy systematically for all field positions with a fixed step size. The simulated actions are logged to a pickle file. 
```
python compare_decision_schemes_newsim.py
```

### Compare Decisions Schemes Random
For a certain amount of random positions on the field the path to the goal is calculated for each strategy and the simulated actions are logged to a pickle file. 
```
compare_decision_schemes_newsim_random.py
```

## Todo
- [ ] das generierte Potentialfeld muss wieder nutzbar gemacht werden  
- [ ] zusammenführen von `compare_decision_schemes_newsim.py` und `compare_decision_schemes_newsim_random.py`  
- [ ] particle filter für den roboter ordentlich implementieren  
