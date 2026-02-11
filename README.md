# SNaiKE - AI Algorithm Testbed

A simulation sandbox built with C++, SFML, and CMake for testing, comparing, and investigating the behavior of classical AI algorithms. SNaiKE uses a snake-based environment as the experimental domain for evaluating algorithm performance and semantic correctness.

> **Status: Work in Progress** - This project is in the midst of a debugging overhaul and algorithmic refinement phase. Core research questions regarding algorithm correctness, behavioral thresholds, and design patterns are actively being addressed.

## Project Overview

SNaiKE is a hotswappable AI algorithm testbed that implements multiple AI strategies operating in a 2D grid-based simulation environment. Rather than focusing on human gameplay, the project investigates deeper questions about what actually constitutes meaningful algorithm behavior: How do theoretical algorithms perform when applied to constrained environments? What environmental factors influence algorithm effectiveness independent of the algorithm's mathematical properties?

### Key Research Areas

The project is currently exploring:

- **Algorithm Semantics**: What truly constitutes the use of an algorithm versus incidental behavior? How do we define the boundary between "following an algorithm" and "environmental adaptation"?
- **Behavioral Thresholds**: What minimum performance criteria should an AI strategy meet to be considered "implementing" a given algorithm?
- **Environmental Constraints**: How do simulator mechanics, grid boundaries, and spatial topology affect algorithm performance independent of the algorithm's mathematical properties?
- **Design Patterns & Testbed Architecture**: Implementing design patterns (Strategy, State Machine, Observer) to create an extensible algorithm evaluation framework

## Test Candidates: Implemented AI Strategies

The testbed includes multiple AI algorithms at varying stages of validation and refinement:

### Classical Algorithms
- **Hamilton Cycle**: Traverses the grid following a Hamiltonian path—a theoretical perfect strategy that must be validated against environment constraints
- **A\* Pathfinding**: Implements A* with configurable heuristics (Manhattan, Euclidean, Chebyshev) for pathfinding-based movement
- **Flood Fill**: Area analysis algorithm to evaluate reachable space and deadlock detection
- **Manhattan Distance**: Greedy baseline using taxicab distance heuristics

### Experimental/Developmental Strategies
- **Basic Strategy**: Foundational greedy baseline for comparison
- **Random Strategy**: Noise baseline for statistical comparison
- **Advanced Strategy**: Hybrid approaches under active development
- **Additional Strategies**: Experimental implementations (Voronoi, Wall Following, Hybrid A*)

## Architecture

The testbed is structured around isolation, experimentation, and extensibility:

- **Strategy Pattern**: AI algorithms implement a common interface enabling hot-swappable test candidates
- **State Machine**: Simulation lifecycle managed through distinct states (Menu, Running, Paused, Results)
- **Observer Pattern**: Event system for decoupling algorithm updates from logging and visualization
- **Factory/Player Pattern**: `AIPlayer` handles strategy instantiation and metric collection

### Directory Structure
```
include/
├── ai/                 # Algorithm implementations and test candidates
├── state_system/       # Simulation state machine and lifecycle management
├── events/             # Event system for testbed communication
├── input/              # User input handling (strategy selection)
└── [core headers]      # Snake (environment), GameLoop, simulation core

src/
├── ai/                 # Algorithm implementations
├── state_system/       # Simulation state handlers
└── [core implementations]
```

## Building the Project

### Prerequisites
- C++17 compatible compiler
- CMake 3.16+
- SFML 2.6+ (automatically fetched via CMake)

### Build Instructions
```bash
cmake -B build
cmake --build build
```

The executable will be located at `build/bin/main`.

### Configuration

See [ORIGINAL_TEMPLATE_README.md](ORIGINAL_TEMPLATE_README.md) for detailed CMake configuration options.

## Running Simulations

```bash
./build/bin/main
```

The simulation starts with a strategy selection menu. Select an algorithm to run the testbed with that strategy. The visualization allows real-time observation of algorithm behavior, decision-making, and performance metrics.

## Development Status

### Current Research Focus
- Algorithm behavior validation against theoretical specifications
- Environmental factor quantification and their effect on algorithm effectiveness  
- Correctness analysis across different grid configurations
- Design pattern implementation for experimental isolation and reproducibility

### Validation & Metrics
A test data file (`hamiltonTestData.txt`) is included for algorithm validation and regression testing during development.

## Research Questions & Experimental Domain

The testbed is designed to explore the gap between theoretical algorithm definitions and real-world constrained environments. Key experimental observations:

- **Environment-Algorithm Coupling**: A mathematically perfect Hamiltonian path is theoretically unbeatable, yet environmental factors (spawn mechanics, body dynamics, boundary constraints) create scenarios where the algorithm's theoretical guarantees don't translate to measured performance
- **Dynamic Obstacle Handling**: Pathfinding algorithms must account for the snake's body becoming obstacles that change every simulation step—a fundamental non-stationarity challenge
- **Grid Topology Effects**: Boundary conditions and spatial constraints fundamentally alter algorithm behavior independent of the algorithm's mathematical properties
- **Behavioral Threshold Definition**: What constitutes "implementing" an algorithm? Is it sufficient to run the decision logic, or must performance meet a minimum threshold?

## Testbed Architecture & Design Patterns

### Core Components
- **Snake**: The simulated entity—defines the environment's constraints, physics, and observable state
- **AIPlayer**: Manages strategy instantiation and execution, collects performance metrics
- **GameLoop**: Simulation engine controlling update frequency and state transitions
- **StateMachine**: Manages simulation lifecycle (initialization, running, paused, results)

### Strategy Implementation Interface
All test candidates inherit from `BaseStrategy` and must implement:
- `Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food)` - Algorithm decision logic
- `void update()` - Internal state management and algorithm-specific bookkeeping
- `void render(sf::RenderWindow& window)` - Visualization of algorithm state and reasoning

## Current Shortcomings & Implementation Status

**Algorithm Correctness Caveat**: Not all implemented strategies should currently be considered correct or production-ready representations of their respective algorithms. The testbed is actively exploring the boundary between algorithmic purity and practical implementation within the Snake simulation.

### Known Issues
- **Several algorithms are in active refactor**: The Hamilton Cycle, A* pathfinding variants, and Flood Fill strategies are undergoing refinement to better align with their theoretical definitions
- **Incomplete Strategy Set**: Some algorithms lack full optimization or deviate from strict algorithmic specifications due to environment constraints
- **Validation in Progress**: Algorithm correctness against theoretical specifications remains under investigation—behavioral validation is ongoing
- **Hybrid Approach Ambiguity**: Some "advanced" strategies blur the line between pure algorithm implementations and heuristic hybrids, requiring clear delineation

### Design Philosophy

The core challenge this project navigates: **How pure can an algorithm implementation be while remaining viable in the Snake environment?** For example:

- A theoretically perfect Hamiltonian cycle may require preprocessing that alters spawn mechanics
- Pathfinding algorithms designed for static graphs must accommodate a dynamically self-modifying obstacle (the snake's body)
- Greedy baselines may outperform more complex algorithms due to environment-specific characteristics

These constraints force deliberate choices about what constitutes a "correct" implementation. The testbed documents these trade-offs explicitly.

## Roadmap

### Phase 1: Algorithm Validation & Correctness (Current)
- [ ] Validate Hamilton Cycle implementation against theoretical guarantees
- [ ] Complete A* implementation with all three heuristic variants functional and testable
- [ ] Establish correctness criteria for each algorithm
- [ ] Document environment-specific adaptations and their justification

### Phase 2: Refactoring & Architecture Cleanup
- [ ] Complete refactor of legacy strategy implementations
- [ ] Unify error handling and state management across all strategies
- [ ] Extract rendering/visualization logic from algorithm logic
- [ ] Improve test isolation and reproducibility

### Phase 3: Metrics & Analysis Framework
- [ ] Implement comprehensive performance metrics collection
- [ ] Add statistical analysis tools for comparing algorithm behavior
- [ ] Create automated regression testing suite
- [ ] Build visualization tools for algorithm decision trees

### Phase 4: Extended Algorithm Set
- [ ] Implement additional classical algorithms (Reinforcement Learning baseline, Genetic Algorithm variant)
- [ ] Explore hybrid approaches with explicit design documentation
- [ ] Add domain-specific optimizations as separate strategy variants
- [ ] Create algorithm comparison benchmarks

### Potential Future Enhancements
- Parameter exploration framework for algorithm tuning
- Multi-agent simulation support
- Different environment topologies (wraparound, obstacles, variable grid sizes)
- ML-based algorithm discovery within the testbed

## License

This project is built upon templates and examples with dual licensing under Public Domain and MIT. Choose whichever you prefer.

---

**Note**: For information about the original CMake SFML template this project was scaffolded from, see [ORIGINAL_TEMPLATE_README.md](ORIGINAL_TEMPLATE_README.md).
