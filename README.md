# polyminis-game-server
Polyminis Game Server


Game Server.

Note this MUST be run inside an Amazon Linux box (what comes with your free tier).

You MUST upgrade your aws cli:
> sudo pip install --upgrade awscli

or the aws_build_upload.sh command will fail.

You MUST set your credentials in aws or the aws_build_upload.sh command will fail. Follow [this](https://s3.amazonaws.com/gamedev-tutorials/Tutorials/GameLift-Getting_started-(02)_Uploading_your_server_build.pdf) instructions to create the policy and user
> aws configure


To build:
> cd build
> ./build.sh

To upload build to amazon:
> cd build
> ./aws_build_upload.sh
