/* ScummVM - Graphic Adventure Engine
*
* ScummVM is the legal property of its developers, whose names
* are too numerous to list here. Please refer to the COPYRIGHT
* file distributed with this source distribution.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef BACKENDS_CLOUD_GOOGLEDRIVE_GOOGLEDRIVEDOWNLOADREQUEST_H
#define BACKENDS_CLOUD_GOOGLEDRIVE_GOOGLEDRIVEDOWNLOADREQUEST_H

#include "backends/cloud/storage.h"
#include "backends/networking/curl/request.h"
#include "common/callback.h"

namespace Cloud {
namespace GoogleDrive {

class GoogleDriveStorage;

class GoogleDriveDownloadRequest: public Networking::Request {
	Common::String _requestedFile, _requestedLocalFile;
	GoogleDriveStorage *_storage;
	Storage::BoolCallback _boolCallback;
	Request *_workingRequest;
	bool _ignoreCallback;

	void start();
	void idResolvedCallback(Storage::UploadResponse response);
	void idResolveFailedCallback(Networking::ErrorResponse error);
	void downloadCallback(Storage::BoolResponse response);
	void downloadErrorCallback(Networking::ErrorResponse error);
	void finishDownload(bool success);
public:
	GoogleDriveDownloadRequest(GoogleDriveStorage *storage, Common::String remotePath, Common::String localPath, Storage::BoolCallback cb, Networking::ErrorCallback ecb);
	virtual ~GoogleDriveDownloadRequest();

	virtual void handle();
	virtual void restart();
};

} // End of namespace GoogleDrive
} // End of namespace Cloud

#endif
