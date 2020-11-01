#ifndef PATHFINDING_HPP
#define PATHFINDING_HPP

#include "MapData.hpp"
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <queue>
#include <cmath>

/// The factor by what speed is reduced in swamps
#define SWAMP_SPEED_PENALITY 10

/** @brief A point in a 2D word used by Pathfinders
  *
  *  @tparam pos   Position of Node in a 2D world.
  *  @tparam field Field-Object a Node is working on.
  *
  *  A %Node can be described as anchor point in a 2D grid.
  *  It offers functions for quick visibility checks for other
  *  Nodes, which can even be in other 2D grids, variables to keep
  *  track of their status in the current pathfinding problem and
  *  a vector containing all visible nodes in the same grid together
  *  with respective their costs.
*/
class Node {
public:
	Node(Point &pos, Field *field);

	/**
	 * @brief Booleans that indicate if this Node is in a special list
	 * used by the A*Pathfinding algorithm
	 * @note These variables aren't updated automatically
	*/
	bool isClosed, isOpen;

	/**
	 * @brief Doubles that are used in the A*Pathfinding algorithm
	 * @note These variables aren't updated automatically
	*/
	double g, f;

	/**
	 * @brief A pointer to the last visited Node. This is used
	 * by the A*Pathfinding algorithm
	 * @note This variable isn't updated automatically
	*/
	Node *previous;

	/**
	 * @brief This Method calculates a cost to a Node
	 * @param node
	 * @details This function takes visibility into account(for that it uses Node::canSee()).
	 * If this Node can't see the other Node the cost will be -1.
	 * Other wise the cost is calculated also taking swamps into account.
	*/
	double calculateCost(Node &node);

	/**
	 * @brief This Method checks if this Node can see a certain other Node
	 * @param node
	 * @param ObstaclesStructs A vector containing all structures that are counted
	 * as Obstacles
	*/
	bool canSee(Node &node, const std::vector<Area> &ObstaclesStructs);

	/**
	 * @brief This Method gets every neighbour and calculates the cost.
	 * @param Nodes A vector of pointers to Nodes
	 * @param ObstaclesStructs A vector containing all structures that are counted
	 * as Obstacles
	 * @return the number of existing neighbours
	 *
	 * @note Every Node in %Nodes has to be initialized before executing this Method
	*/
	int findNeighbours(const std::vector<Node> &Nodes, const std::vector<Area> &ObstacleStructs);

	/// Getter for Node::pos_
	const Point &getPos() const;

	/// Getter for Node::Field_
	Field *getField() const;

	/// Getter for Node::neighbours_
	const std::vector<std::pair<Node *, double>> &getNeighbours() const;

private:

	/**
	 * @brief A Point struct that stores the position of this node
	 * @note This variable could be constant and is not meant to change.
	 * However for useability reasons it's not constant.
	*/
	Point pos_{};

	/**
	 * @brief A pointer to the Field Object this Node is storred in
	 * @details This pointer can be used to find neighbours or obstacles.
	 * It is important that a node knows in which Field it is operating in.
	 *
	 * @note This variable could be constant and is not meant to change.
	 * However for useability reasons it's not constant.
	 *
	 * @attention Maybe theres a better solution to this
	*/
	Field *Field_;

	/**
	 * @brief A vector that stores all visible neighbour nodes with their
	 * respective costs.
	 * @details To optimise both speed and memory usage the A*Pathfinding
	 * works on a precalculated enviroment. This vector keeps track of
	 * neighbours and distances/costs.
	*/
	std::vector<std::pair<Node *, double>> neighbours_; // Node / cost

};


// distance between two nodes



class Pathfinder {
public:
	Pathfinder(Field &MAP, bool trap_sensitive);

	std::vector<Node> map;

	// pathfinding algorithm
	std::vector<Point> AStar(Point &start, Point &end);

private:

	bool trap_sensitive_;
	Field *Field_;

	struct PRIORITY {
		bool operator()(Node *child, Node *parent) const {
			return parent->f < child->f;
		}
	};

	/// heuristic function
	static double heuristic(const Point &cur, const Point &end);

	// convert previous pointers of nodes to path
	static std::vector<Node> traverse(Node *end);

	// convert nodepath to pair
	static std::vector<Point> to_point(const std::vector<Node> &p);
};

namespace helper {
	bool compare(const std::pair<Point, double> &p, const std::pair<Point, double> &q);
}

#endif //PATHFINDING_HPP