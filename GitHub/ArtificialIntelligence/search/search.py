# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def search_util(problem, s):
    startState = problem.getStartState()
    node = [startState, None, 0, None]
    s.push(node)
    visited = []
    while not s.isEmpty():
        currentNode = s.pop()
        if visited.__contains__(currentNode[0]):
            continue
        
        visited.append(currentNode[0])
        
        if problem.isGoalState(currentNode[0]):
            seq = []
            for i in range(0, currentNode[2]):
                seq.append(0)
            node = currentNode
            while node is not None:
                if node[3] == None:
                    break
                seq[node[2]-1] = node[3]
                node = node[1]
            
            return seq
        
        l = problem.getSuccessors(currentNode[0])
        depth = currentNode[2]
        for i in range(len(l)):
            childNode = [l[len(l)-i-1][0], currentNode, depth+1, l[len(l)-i-1][1]]
            s.push(childNode)

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    """

    s = util.Stack()
    return search_util(problem, s)

def breadthFirstSearch(problem):
    s = util.Queue()
    return search_util(problem, s)

def uniformCostSearch(problem):
    s = util.PriorityQueue()
    startState = problem.getStartState()
    node = [startState, None, 0, None, 0]
    s.push(node, 0)
    visited = []
    while not s.isEmpty():
        currentNode = s.pop()
        if visited.__contains__(currentNode[0]):
            continue
        
        visited.append(currentNode[0])
        
        if problem.isGoalState(currentNode[0]):
            seq = []
            for i in range(0, currentNode[2]):
                seq.append(0)
            node = currentNode
            while node is not None:
                if node[3] == None:
                    break
                seq[node[2]-1] = node[3]
                node = node[1]
            
            return seq
        
        l = problem.getSuccessors(currentNode[0])
        depth = currentNode[2]
        for tempState in l:
            cost = tempState[2] + currentNode[4]
            childNode = [tempState[0], currentNode, depth+1, tempState[1], cost]
            s.push(childNode, cost)

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    s = util.PriorityQueue()
    startState = problem.getStartState()
    h_state = heuristic(startState, problem)
    
    node = [startState, None, 0, None, 0]
    visited = []
    s.push(node, h_state)
    while not s.isEmpty():
        currentNode = s.pop()
        if visited.__contains__(currentNode[0]):
            continue
        
        visited.append(currentNode[0])
        
        if problem.isGoalState(currentNode[0]):
            seq = []
            for i in range(0, currentNode[2]):
                seq.append(0)
            node = currentNode
            while node is not None:
                if node[3] == None:
                    break
                seq[node[2]-1] = node[3]
                node = node[1]
            
            return seq
        
        l = problem.getSuccessors(currentNode[0])
        depth = currentNode[2]
        for tempState in l:
            cost = tempState[2] + currentNode[4]
            h_value = heuristic(tempState[0], problem)
            childNode = [tempState[0], currentNode, depth+1, tempState[1], cost]
            s.push(childNode, cost + h_value)
    
    return None

# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
