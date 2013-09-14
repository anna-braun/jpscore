/**
 * @file    Goal.cpp
 * @author  Ulrich Kemloh <kemlohulrich@gmail.com>
 * Created on: Sep 12, 2013
 * Copyright (C) <2009-2013>
 *
 * @section LICENSE
 * This file is part of JuPedSim.
 *
 * JuPedSim is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * JuPedSim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with JuPedSim. If not, see <http://www.gnu.org/licenses/>.
 *
 * @section DESCRIPTION
 *
 *
 *
 */

#include "Goal.h"
#include "Wall.h"
#include "Point.h"


using namespace std;


Goal::Goal() {
	_id=-1;
	_caption="Goal";
	_isFinalGoal=0;
	_walls = vector<Wall > ();
	_poly = vector<Point > ();
}

Goal::~Goal() {

}

void Goal::AddWall(const Wall& w) {
	_walls.push_back(w);
}

string Goal::GetCaption() const {
	return _caption;
}

void Goal::SetCaption(string caption) {
	_caption = caption;
}

int Goal::GetId() const {
	return _id;
}

void Goal::SetId(int id) {
	_id = id;
}

const vector<Point>& Goal::GetPolygon() const {
	return _poly;
}

string Goal::Write() {
	string s;
	Point pos;

	for (unsigned int j = 0; j < _walls.size(); j++) {
		const Wall& w = _walls[j];
		s.append(w.Write());
		pos = pos + w.GetPoint1() + w.GetPoint2();
	}
	pos = pos * (0.5 / _walls.size());

	// add some fancy stuffs
	if(_poly.size()>=4){
		s.append(Wall(_poly[0],_poly[2]).Write());
		s.append(Wall(_poly[1],_poly[3]).Write());
	}
	//add the Goal caption
	char tmp[CLENGTH];
	sprintf(tmp, "\t\t<label centerX=\"%.2f\" centerY=\"%.2f\" centerZ=\"0\" text=\"%s\" color=\"100\" />\n"
			, pos.GetX() * FAKTOR, pos.GetY() * FAKTOR, _caption.c_str());
	s.append(tmp);

	return s;
}

const vector<Wall>& Goal::GetAllWalls() const {
	return _walls;
}

int Goal::WhichQuad(const Point& vertex, const Point& hitPos) const {
	return (vertex.GetX() > hitPos.GetX()) ? ((vertex.GetY() > hitPos.GetY()) ? 1 : 4) :
			((vertex.GetY() > hitPos.GetY()) ? 2 : 3);

}

int Goal::GetIsFinalGoal() const {
	return _isFinalGoal;
}

void Goal::SetIsFinalGoal(int isFinalGoal) {
	_isFinalGoal = isFinalGoal;
}

// x-Koordinate der Linie von einer Eccke zur nächsten
double Goal::Xintercept(const Point& point1, const Point& point2, double hitY) const {
	return (point2.GetX() - (((point2.GetY() - hitY) * (point1.GetX() - point2.GetX())) /
			(point1.GetY() - point2.GetY())));
}


bool Goal::Contains(const Point& ped) const {


	short edge, first, next;
	short quad, next_quad, delta, total;

	/////////////////////////////////////////////////////////////
	edge = first = 0;
	quad = WhichQuad(_poly[edge], ped);
	total = 0; // COUNT OF ABSOLUTE SECTORS CROSSED
	/* LOOP THROUGH THE VERTICES IN A SECTOR */
	do {
		next = (edge + 1) % _poly.size();
		next_quad = WhichQuad(_poly[next], ped);
		delta = next_quad - quad; // HOW MANY QUADS HAVE I MOVED

		// SPECIAL CASES TO HANDLE CROSSINGS OF MORE THEN ONE
		//QUAD

		switch (delta) {
			case 2: // IF WE CROSSED THE MIDDLE, FIGURE OUT IF IT
				//WAS CLOCKWISE OR COUNTER
			case -2: // US THE X POSITION AT THE HIT POINT TO
				// DETERMINE WHICH WAY AROUND
				if (Xintercept(_poly[edge], _poly[next], ped.GetY()) > ped.GetX())
					delta = -(delta);
				break;
			case 3: // MOVING 3 QUADS IS LIKE MOVING BACK 1
				delta = -1;
				break;
			case -3: // MOVING BACK 3 IS LIKE MOVING FORWARD 1
				delta = 1;
				break;
		}
		/* ADD IN THE DELTA */
		total += delta;
		quad = next_quad; // RESET FOR NEXT STEP
		edge = next;
	} while (edge != first);

	/* AFTER ALL IS DONE IF THE TOTAL IS 4 THEN WE ARE INSIDE */
	if (abs(total) == 4)
		return true;
	else
		return false;
}

void Goal::ConvertLineToPoly() {

	vector<Line*> copy;
	vector<Point> tmpPoly;
	Point point;
	Line* line;
	// Alle Linienelemente in copy speichern
	for (unsigned int i = 0; i < _walls.size(); i++) {
		copy.push_back(&_walls[i]);
	}

	line = copy[0];
	tmpPoly.push_back(line->GetPoint1());
	point = line->GetPoint2();
	copy.erase(copy.begin());
	// Polygon aus allen Linen erzeugen
	for (int i = 0; i < (int) copy.size(); i++) {
		line = copy[i];
		if ((point - line->GetPoint1()).Norm() < J_TOLERANZ) {
			tmpPoly.push_back(line->GetPoint1());
			point = line->GetPoint2();
			copy.erase(copy.begin() + i);
			// von vorne suchen
			i = -1;
		} else if ((point - line->GetPoint2()).Norm() < J_TOLERANZ) {
			tmpPoly.push_back(line->GetPoint2());
			point = line->GetPoint1();
			copy.erase(copy.begin() + i);
			// von vorne suchen
			i = -1;
		}
	}
	if ((tmpPoly[0] - point).Norm() > J_TOLERANZ) {
		char tmp[CLENGTH];
		sprintf(tmp, "ERROR: \tGoal::ConvertLineToPoly(): ID %d !!!\n", _id);
		Log->Write(tmp);
		exit(0);
	}
	_poly = tmpPoly;
}
