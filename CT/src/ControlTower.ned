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
simple ControlTower{
    parameters:
        @signal[LandingQueueTime](type=double);
		@statistic[LandingQueueTime](source="LandingQueueTime"; record=vector;);
		@signal[TakeoffQueueTime](type=double);
		@statistic[TakeoffQueueTime](source="TakeoffQueueTime"; record=vector;);
    gates:
        input inAS;
        input inPA;
        output outPA;
}
