#pragma once


class Unit;


class Map {
public:			// Construction and destruction
	Map(unsigned width, unsigned height);
	virtual ~Map();

public:			// Unit interaction interface
	void AddUnit(Unit * unit);

	template <typename FilterT>
	void GetUnitsByPosition(unsigned x, unsigned y, const FilterT & filter, std::vector<const Unit *> * out) const {
		out->clear();
		for(auto & unit : m_ownedUnits) {
			auto & pos = unit->GetCoordinates();
			if(pos.x == x && pos.y == y && filter(unit))
				out->push_back(unit);
		}
	}

private:		// Internal state
	std::vector<Unit *> m_ownedUnits;
};


