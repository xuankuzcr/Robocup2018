//实现各种扑倒挡球的功能
option(Keeper2017Simple)
{
	const Vector2f ball = theBallModel.estimate.position;
	const Vector2f globalBall = Transformation::robotToField(theRobotPose, ball);

   
	initial_state(start)
	{
		transition
		{
			if (state_time > 3000)
				  goto judgeState;
		}
			action
		{
			LookRound();
		}

	}
	state(judgeState) {
		transition{
			if (globalBall.x() < theFieldDimensions.xPosHalfWayLine)
                goto danger;
			else
				goto safe;
		}
		action{
			Stand();
			LookRound();
		}
	}
	//球未过半场即为safe
	state(safe)
	{
		transition
		{

			if (globalBall.x() < theFieldDimensions.xPosHalfWayLine)
				  goto danger;
		}
			action
		{   //WalkToTargetAbsolute(Pose2f(0.f,0.f,0.f),Pose2f(0.f,-4000.f,0.0f));
		  //若机器人未处于球门中央位置，则走至中央位置
			if (std::abs(theRobotPose.translation.y() > 50)
				|| std::abs(theRobotPose.translation.x() - theFieldDimensions.xPosOwnGroundline-200) > 50 ) {
				WalkToTarget(Pose2f(0.2f, 0.6f, 0.5f), Pose2f(libCodeRelease.angleToGoal, theFieldDimensions.xPosOwnGroundline+200-theRobotPose.translation.x() ,0-theRobotPose.translation.y()  ));
		  }
          else 
			Stand();
			LookForward();
            KeyFrameArms(ArmKeyFrameRequest::back);
		}
	}
	//球到达己方半场时，进入danger状态
	state(danger)
	{
		//计算球的速度和位置，球门线交叉点的时间及位置
		Vector2f vBall = theBallModel.estimate.velocity ++;
 
		transition
		{

		    if (libCodeRelease.timeSinceBallWasSeen() > 5000)
				goto searchForBall;
			if (globalBall.x() > theFieldDimensions.xPosHalfWayLine)
				goto safe;


            int location = 0;
            if(globalBall.x() < theFieldDimensions.xPosOwnPenaltyMark+400){
                if(std::abs(globalBall.y()) < theFieldDimensions.yPosLeftPenaltyArea){

                    if(vBall.norm() < 5.0f 
                    && globalBall.x() < theFieldDimensions.xPosOwnPenaltyArea
                    && std::abs(theRobotPose.translation.y()) < 1100
                    && theRobotPose.translation.x() < theFieldDimensions.xPosOwnPenaltyArea+100){
                        goto attack;
                    }else if(vBall.x() < -1.0f){
                        goto middelSideDown;
                    }
                }

            }
			
        }
			action
		{ if (std::abs(theRobotPose.translation.y()) > 1100
				||theRobotPose.translation.x() > theFieldDimensions.yPosLeftPenaltyArea+200 ) {
				WalkToTarget(Pose2f(0.2f, 0.6f, 0.5f), Pose2f(libCodeRelease.angleToGoal, theFieldDimensions.xPosOwnGroundline+100-theRobotPose.translation.x() ,0-theRobotPose.translation.y()  ));
		  }
        else if(globalBall.y() < -200){
            
           if (theRobotPose.translation.y() > -100.0f
				|| std::abs(theRobotPose.translation.x()-200 - theFieldDimensions.xPosOwnGroundline) > 50 )
            {
			   WalkToTarget(Pose2f(0.4f, 0.8f, 0.8f), Pose2f(theBallModel.estimate.position.angle(),theFieldDimensions.xPosOwnGroundline+200-theRobotPose.translation.x() ,-300.0f-theRobotPose.translation.y() ));
            }

		}else if(globalBall.y() > 200){
           if (theRobotPose.translation.y() < 100.0f
				|| std::abs(theRobotPose.translation.x()-200 - theFieldDimensions.xPosOwnGroundline) > 50 )
            {
			   WalkToTarget(Pose2f(0.4f, 0.8f, 0.8f), Pose2f(theBallModel.estimate.position.angle(),theFieldDimensions.xPosOwnGroundline+200-theRobotPose.translation.x() ,300.0f-theRobotPose.translation.y() ));
            }            
        }
        else {
           if (std::abs(theRobotPose.translation.y()) < 100.0f
				|| std::abs(theRobotPose.translation.x()-200 - theFieldDimensions.xPosOwnGroundline) > 50 ) 
            {
			   WalkToTarget(Pose2f(0.4f, 0.8f, 0.8f), Pose2f(theBallModel.estimate.position.angle(),theFieldDimensions.xPosOwnGroundline+200-theRobotPose.translation.x() ,0.0f-theRobotPose.translation.y() ));
            }        
        }
	}
        LookForward();
    }
	//主动出击
	state(attack)
	{
		transition
		{
			if (libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
			    goto searchForBall;
            if (globalBall.x() > theFieldDimensions.xPosHalfWayLine)
				goto safe;
            if(theRobotPose.translation.x() > theFieldDimensions.xPosOwnPenaltyArea)
                goto judgeState;
           if(std::abs(theRobotPose.translation.y()) > theFieldDimensions.yPosLeftPenaltyArea)
                goto judgeState;     
					
		}
			action
		{
                if(ball.norm() < 500.f){
                    LookForward();
                    if (libCodeRelease.angleToGoal >= -60_deg&& libCodeRelease.angleToGoal <= 60_deg)
                    {
                        static int lastballpos=0;
                        if(std::abs(ball.y())>20.f)
                                lastballpos=0;
                        if(lastballpos==1||ball.y()>0)
                        {
                            //WalkToTarget(Pose2f(10.f, 50.f, 50.f), 
                            //              Pose2f(libCodeRelease.angleToGoal,ball.x() - 150.f, ball.y() -20.f));
                            if (libCodeRelease.between(ball.x(), 140.f, 170.f) && libCodeRelease.between(ball.y(), 20.f, 50.f))
                            {
                                InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), 
                                            Pose2f(libCodeRelease.angleToGoal - 8_deg,ball.x() - 160.f, ball.y() - 55.f));
                                //LeftKickForward();                                            160.f, ball.y() - 55.f));
                                //减了15度；
                                
                                lastballpos=0;
                            }
                            else
                                {WalkToTarget(Pose2f(0.5f, 0.5f, 0.5f), 
                                                Pose2f(libCodeRelease.angleToGoal - 8_deg,ball.x() - 150.f, ball.y() -30.f));
												//                                        150.f, ball.y() -30.f)
                                lastballpos=1;}
                        }
                        else if(lastballpos==2 || ball.y()<=0)
                        {
                            if (libCodeRelease.between(ball.x(), 140.f, 170.f) && libCodeRelease.between(ball.y(), -50.f, -20.f))
                            {
                                InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::right), 
                                            Pose2f(libCodeRelease.angleToGoal + 8_deg,ball.x() - 160.f, ball.y() + 55.f));
                                lastballpos=0;
                                                  }
                            else
                               {WalkToTarget(Pose2f(0.5f, 0.5f, 0.5f), 
                                                Pose2f(libCodeRelease.angleToGoal + 8_deg,ball.x() - 150.f, ball.y() +30.f));
							   lastballpos=2;}
                        }
                    }
                    else if (libCodeRelease.angleToGoal>60_deg && libCodeRelease.angleToGoal<180_deg)
                    {
                        if (libCodeRelease.between(ball.x(), 160.f, 180.f) && 
                                        libCodeRelease.between(ball.y(), 10.f, 30.f))
                        {
                               InWalkKick(WalkKickVariant(WalkKicks::sidewardsInner, Legs::right), 
                                       Pose2f(libCodeRelease.angleToGoal-90_deg,ball.x() - 170.f, ball.y() - 12.f));
                                
                        }
                        else
                            WalkToTarget(Pose2f(0.5f, 0.5f, 0.5f), 
                                            Pose2f(libCodeRelease.angleToGoal-90_deg,ball.x() - 170.f, ball.y() - 20.f));
											//                                          150.f, ball.y() - 20.f));
                    }
                    else if (libCodeRelease.angleToGoal>-180_deg && libCodeRelease.angleToGoal<-60_deg)
                    {
                        if (libCodeRelease.between(ball.x(), 160.f, 180.f) && 
                                    libCodeRelease.between(ball.y(), -30.f, -10.f) )
                            {
                                InWalkKick(WalkKickVariant(WalkKicks::sidewardsInner, Legs::left), 
                                            Pose2f(libCodeRelease.angleToGoal+90_deg,ball.x() - 170.f, ball.y() + 12.f));
                        //                                                                 150.f, ball.y() + 15.f));
                                
                            }
                        else 
                            WalkToTarget(Pose2f(0.5f, 0.5f, 0.5f), 
                                            Pose2f(libCodeRelease.angleToGoal+90_deg,ball.x() - 170.f, ball.y() + 20.f));
											//                                                  150.f, ball.y() + 20.f));   
                    }
                }
                else
                    WalkToTarget(Pose2f(0.5f, 0.5f, 0.5f), Pose2f(ball.angle(), ball.x(), ball.y()));
		}
	}
	//倒身
	state(leftSideDown)
	{
		transition
		{
			if (state_time > 6000.f)
				  goto getUp;
		}

			action
		{
		   KickForward(KickRequest::leftSideDown);
		}
	}
	state(rightSideDown)
	{
		transition
		{
			if (state_time > 6000.f)
				  goto getUp;
		}

			action
		{
			KickForward(KickRequest::rightSideDown);
		}
	}
	state(middelSideDown)
	{
		transition
		{
			if (state_time > 2000.f)
				  goto getUp;
		}

			action
		{
		//KickForward(KickRequest::down);
		SpecialAction(SpecialActionRequest::new1);
        }
	}
	state(KgetUp)
	{
		transition
		{
			if (state_time > 6000.f)
				goto KFgetUp;
		}
			action
		{
			GetUp();
		}
	}
	state(KFgetUp)
	{
		transition
		{
			if (state_time > 10000.f)
				goto judgeState;
            //if (libCodeRelease.timeSinceBallWasSeen() < 300)
			//goto judgeState;
		}
			action
		{
WalkAtSpeedPercentage(Pose2f(1.f, 0.f, 0.f));
		}
	}    
	state(getUp)
	{
		transition
		{
			if (state_time > 3000.f)
				goto judgeState;
		}
			action
		{
			GetUp();
		}
	}

	state(searchForBall)
	{
		transition
		{
		  if (libCodeRelease.timeSinceBallWasSeen() < 300)
			goto judgeState;
            
		}
			action
		{

        /*    if(state_time % 3 == 0)
                SetHeadPanTilt(1.f, 0.38f, 1.5f);
            else if(state_time % 3 == 1)
                SetHeadPanTilt(0.f, 0.38f, 1.5f);
            else if(state_time % 3 == 2)
                SetHeadPanTilt(1.f, 0.38f, 1.5f);
          */
          if (std::abs(theRobotPose.translation.y()) > 50
				|| std::abs(theRobotPose.translation.x() - theFieldDimensions.xPosOwnGroundline) > 50 ) {
				WalkToTarget(Pose2f(0.2f, 0.6f, 0.5f), Pose2f(libCodeRelease.angleToGoal, theFieldDimensions.xPosOwnGroundline+100-theRobotPose.translation.x() ,0-theRobotPose.translation.y()  ));
		  }
            LookRound(0.5f);
            //else 
              //  LookForward();
		}
	}

	state(KturnToBall)
	{
		transition
		{
			if (libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
			    goto searchForBall;
            if (globalBall.x() > theFieldDimensions.xPosHalfWayLine)
				goto safe;
            if(theRobotPose.translation.x() > theFieldDimensions.xPosOwnPenaltyArea)
                goto judgeState;
           if(std::abs(theRobotPose.translation.y()) > theFieldDimensions.yPosLeftPenaltyArea)
                goto judgeState;     
		if (std::abs(theBallModel.estimate.position.angle()) < 5_deg)
			goto KwalkToBall;
		}
			action
		{
			LookForward();
            KeyFrameArms(ArmKeyFrameRequest::back);
		WalkToTarget(Pose2f(0.8f, 0.8f, 0.8f), Pose2f(theBallModel.estimate.position.angle(), 0.f, 0.f));
		}
	}

	state(KwalkToBall)
	{
		transition
		{
			if (libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
			goto searchForBall;
            if (globalBall.x() > theFieldDimensions.xPosHalfWayLine)
				goto safe;
           if(theRobotPose.translation.x() > theFieldDimensions.xPosOwnPenaltyArea)
                goto judgeState;
           if(std::abs(theRobotPose.translation.y()) > theFieldDimensions.yPosLeftPenaltyArea)
                goto judgeState;     
		if (theBallModel.estimate.position.norm() < 500.f)
			goto KalignBehindBall;
        if (std::abs(theRobotPose.translation.y() )> theFieldDimensions.yPosLeftPenaltyArea
				|| theRobotPose.translation.x()  > theFieldDimensions.xPosOwnPenaltyArea ) {
            goto judgeState;
		  }
		}
			action
		{
			LookForward();
            KeyFrameArms(ArmKeyFrameRequest::back);
		WalkToTarget(Pose2f(0.2f, 0.6f, 0.5f), theBallModel.estimate.position);
		}
	}

	state(KalignToGoal)
	{
		transition
		{
			if (libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
			goto searchForBall;
            if (globalBall.x() > theFieldDimensions.xPosHalfWayLine)
				goto safe;
              if(theRobotPose.translation.x() > theFieldDimensions.xPosOwnPenaltyArea)
                goto judgeState;
           if(std::abs(theRobotPose.translation.y()) > theFieldDimensions.yPosLeftPenaltyArea)
                goto judgeState;     
		if (std::abs(libCodeRelease.angleToGoal) < 10_deg && std::abs(theBallModel.estimate.position.y()) < 100.f)
			goto KalignBehindBall;
		}
			action
		{
			LookForward();
		WalkToTarget(Pose2f(0.8f, 0.8f, 0.8f), Pose2f(theBallModel.estimate.position.angle(), theBallModel.estimate.position.x() - 400.f, theBallModel.estimate.position.y()));
		}
	}

	state(KalignBehindBall)
	{
		transition
		{
			if (libCodeRelease.timeSinceBallWasSeen() > theBehaviorParameters.ballNotSeenTimeOut)
			goto searchForBall;
		if (libCodeRelease.between(theBallModel.estimate.position.y(), 20.f, 50.f)
			&& libCodeRelease.between(theBallModel.estimate.position.x(), 155.f, 185.f)
			&& std::abs(libCodeRelease.angleToGoal) < 2_deg)
			goto kick;
            if(theRobotPose.translation.x() > theFieldDimensions.xPosOwnPenaltyArea)
                goto judgeState;
           if(std::abs(theRobotPose.translation.y()) > theFieldDimensions.yPosLeftPenaltyArea)
                goto judgeState;     
                    if (std::abs(theRobotPose.translation.y() )> theFieldDimensions.yPosLeftPenaltyArea
				||   theRobotPose.translation.x()  > theFieldDimensions.xPosOwnPenaltyArea ) {
            goto judgeState;
		  }
		}
			action
		{
			LookForward();
            KeyFrameArms(ArmKeyFrameRequest::back);
		WalkToTarget(Pose2f(0.8f, 0.8f, 0.8f), Pose2f(libCodeRelease.angleToGoal, theBallModel.estimate.position.x() - 165.f, theBallModel.estimate.position.y() - 30.f));
		}
	}

	state(kick)
	{
		transition
		{
			if (state_time > 3000 || (state_time > 10 && action_done))
			goto judgeState;
		}
			action
		{
		//KickForward(KickRequest::l)
		//KickForward(KickRequest::leftKickForward);
		/*    kickForward,
		leftKickFoward,
		* leftKickForward
		rightKickFoward,
		leftSideDown,
		rightSideDown,
		throwDown,
		throwDownG,*/
        KeyFrameArms(ArmKeyFrameRequest::back);
		InWalkKick(WalkKickVariant(WalkKicks::forward, Legs::left), Pose2f(theBallModel.estimate.position.angle(), theBallModel.estimate.position.x() - 160.f, theBallModel.estimate.position.y() - 55.f));
		}
	}
    
    state()


}