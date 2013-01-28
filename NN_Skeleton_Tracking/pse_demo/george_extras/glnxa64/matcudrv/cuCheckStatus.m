function ret = cuCheckStatus(status,message)
%cuCheckStatus  Check the CUDA DRV status.
%  cuCheckStatus(STATUS,MSG) returns EXIT_FAILURE(1) or
%  EXIT_SUCCESS(0) depending on STATUS value, and throws an
%  error with message 'MSG'.

% global GPUcontext
% 
% if (isempty(GPUcontext))
%   error('GPU not started. Please start the GPU using GPUstart');
% end

%
%     Copyright (C) 2012  GP-you Group (http://gp-you.org)
% 
%     This program is free software: you can redistribute it and/or modify
%     it under the terms of the GNU General Public License as published by
%     the Free Software Foundation, either version 3 of the License, or
%     (at your option) any later version.
% 
%     This program is distributed in the hope that it will be useful,
%     but WITHOUT ANY WARRANTY; without even the implied warranty of
%     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%     GNU General Public License for more details.
% 
%     You should have received a copy of the GNU General Public License
%     along with this program.  If not, see <http://www.gnu.org/licenses/>.


EXIT_FAILURE	= 1;
EXIT_SUCCESS	= 0;


ret = EXIT_SUCCESS;


if (status ~= 0)
    ret = EXIT_FAILURE;
    error(message);
end

