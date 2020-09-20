#include <ros/ros.h>
#include <ros/console.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>
#include <visualization_msgs/Marker.h>

//Positions
float pickUp[3] = {3.3, 2.5, 1.0};
float dropOff[3] = {7.0, 1.0, 1.0};

double robot_x, robot_y;
double DISTANCE_THRESHOLD = 0.5;

void robotPoseCallback(const geometry_msgs::PoseWithCovarianceStamped::ConstPtr &msg_amcl)
{
  // Update robot position
  robot_x = msg_amcl->pose.pose.position.x;
  robot_y = msg_amcl->pose.pose.position.y;
}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "basic_shapes");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  visualization_msgs::Marker marker;
  // Set our initial shape type to be a cube
    uint32_t shape = visualization_msgs::Marker::CUBE;
  // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "map";
    marker.header.stamp = ros::Time::now();
  // Set the namespace and id for this marker.  This serves to create a unique ID
  // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;
  // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;
  // Set marker orientation
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
  // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;
  // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = 1.0f;
    marker.color.b = 0.0f;
    marker.color.a = 1.0;
  marker.pose.position.z = 0;
  marker.lifetime = ros::Duration();

  // Subscribe to /amcl_pose
  ros::Subscriber sub1 = n.subscribe("/amcl_pose", 1000, robotPoseCallback);

  int state = 0 , current_wait = 0 ;

  while (ros::ok())
  {
    
 // State transitions

    if (state == 0)
    {
      // Calculate manhattan distance
      double pickup_distance = abs(robot_x - pickUp[0]) + abs(robot_y - pickUp[1]);

      if (pickup_distance > DISTANCE_THRESHOLD)
      {
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = pickUp[0];
        marker.pose.position.y = pickUp[1];
        ROS_INFO("X: %lf  Y: %lf",robot_x,robot_y);

      }
      else
      {
        state = 1;
        marker.action = visualization_msgs::Marker::DELETE;
        ROS_INFO(" Robot reached PICK-UP");

      }
    }

    else if (state == 1)
    {
      if (current_wait < 5)
      {
        current_wait += 1;
      }
      else
      {
        state = 2;
      }
    }

    else if (state == 2)
    {
      // Calculate manhattan distance
      double dropoff_distance = abs(robot_x - dropOff[0]) + abs(robot_y - dropOff[1]);

      if (dropoff_distance > DISTANCE_THRESHOLD)
      {
        marker.action = visualization_msgs::Marker::DELETE;
      }
      else
      {
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = dropOff[0];
        marker.pose.position.y = dropOff[1];
        ROS_INFO("Robot reached DROP OFF");

      }
    }

    // Publish the Marker
    marker_pub.publish(marker);

    // Sleep for 1 seconds
    sleep(1);

    // Handle ROS communication events
    ros::spinOnce();
  
  }
}