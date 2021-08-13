//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package controltower.simulations;

import Airspace;
import ControlTower;
import ParkingArea;

network Airport
{
    @display("bgb=503,274");
    submodules:
        airspace: Airspace {
            @display("p=406,120");
        }
        controlTower: ControlTower {
            @display("p=251,120");
        }
        parkingArea: ParkingArea {

            @display("p=76,120");
        }
    connections:
        airspace.outCT --> controlTower.inAS;

        controlTower.outPA --> parkingArea.inCT;
        controlTower.inPA <-- parkingArea.outCT;
}
