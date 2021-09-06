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

//
// TODO auto-generated module
//
simple Airspace
{
    parameters:
        string interarrivalDistr = default("constant");
        string landingDistr = default("constant");
    	string parkingDistr = default("constant");
    	string takeoffDistr = default("constant");
        double interarrivalTimeAvg = default(900);
        double landingTimeAvg = default(900);
        double parkingTimeAvg = default(900);
        double takeoffTimeAvg = default(900);
        int interarrivalRNGIdx = default(0);
        int landingRNGIdx = default(0);
        int parkingRNGIdx = default(0);
        int takeoffRNGIdx = default(0);
    gates:
        output outCT;
}
