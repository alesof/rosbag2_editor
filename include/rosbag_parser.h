#pragma once

#include <rosbag2_cpp/reader.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>
#include <rosbag2_cpp/readers/sequential_reader.hpp>
#include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>

/*
Define all helpers as class members of Rosbag2Parser
*/

class Rosbag2Parser
{
public:
    Rosbag2Parser(const std::string &bagPath, const std::string &i_ser = "cdr", const std::string &o_ser = "cdr")
    {

        converter_options_.input_serialization_format = i_ser;
        converter_options_.output_serialization_format = o_ser;
        bag_path_ = bagPath;

        openBag();
    };
    ~Rosbag2Parser(){};

    void setBagPath(const std::string &bagPath) { bag_path_ = bagPath; }
    const std::string &getBagPath() { return bag_path_; }

    void convert2csv(std::string outputDir)
    {

        while (reader_.has_next())
        {
            auto serialized_message = reader_.read_next();
            auto topic_name = serialized_message->topic_name;
            auto message_time = serialized_message->time_stamp;
            std::cout << "Topic: " << topic_name << " time_stamp: " << message_time << std::endl;
        }
    };

    void openBag(bool verbose = false)
    {

        storage_options_.uri = bag_path_;

        try
        {
            reader_.open(storage_options_, converter_options_);

            if (verbose)
            {
                const auto metadata = reader_.get_metadata();
                // std::cout << "Bag duration: " << metadata.duration;
            }

            std::vector<rosbag2_storage::TopicMetadata> topics_and_types_ = reader_.get_all_topics_and_types();

            for (const auto & topic : topics_and_types_)
            {
                std::cout<<"Populating topic map"<<std::endl;
                topic_name_map_[topic.name]=topic.type;
            }

        }
        catch (const std::exception &e)
        {
            std::cerr << "Error opening bag: " << e.what() << std::endl;
        }

    }

    void closeBag()
    {
        reader_.close();
    }

    std::shared_ptr<rosbag2_storage::SerializedBagMessage> readNext()
    {

        if (!reader_.has_next())
        {
            std::cout << "No more messages to read." << std::endl;
            return nullptr;
        }

        msg_ = reader_.read_next();
        return msg_;
    }

    rosbag2_storage::BagMetadata getMetadata()
    {
        return reader_.get_metadata();
    }

    bool hasNext()
    {
        return reader_.has_next();
    }

    void bag2csv(){

        auto ros_message = std::make_shared<rosbag2_cpp::rosbag2_introspection_message_t>();
        std::shared_ptr<rosbag2_storage::SerializedBagMessage> serialized_message;
        
        std::unique_ptr<rosbag2_cpp::converter_interfaces::SerializationFormatDeserializer> cdr_deserializer;
        cdr_deserializer = factory_.load_deserializer("cdr");
        std::string topic_type;

        //TODO:
        // 1. Get typesupport for specific topic
        // 2. Deserialize message
        // 3. Write to CSV

        while(reader_.has_next()){

            serialized_message = reader_.read_next();
            topic_type = topic_name_map_[serialized_message->topic_name];

            //TEST BECAUSE I KNOW THE TOPIC IS GEOMETRY_MSGS/POSE
            geometry_msgs::msg::Pose pose_test;
            ros_message->message = &pose_test;
            
            auto library_test = rosbag2_cpp::get_typesupport_library(topic_type, "rosidl_typesupport_cpp");
            auto type_support_pose = rosbag2_cpp::get_typesupport_handle(topic_type, "rosidl_typesupport_cpp", library_test);

            cdr_deserializer->deserialize(serialized_message, type_support_pose, ros_message);
            std::cout << "POSE" << "," << pose_test.position.x << "," << pose_test.position.y << "," << pose_test.position.z << "," << pose_test.orientation.x << "," << pose_test.orientation.y << "," << pose_test.orientation.z << "," << pose_test.orientation.w << std::endl;

        }

    // while (reader.has_next()) {
        
    //     serialized_message = reader.read_next();

    //     auto general_lib = rosbag2_cpp::get_typesupport_library(topicNameMap[serialized_message->topic_name], "rosidl_typesupport_cpp");
    //     auto general_type_support = rosbag2_cpp::get_typesupport_handle(topicNameMap[serialized_message->topic_name], "rosidl_typesupport_cpp", general_lib);
        
    //     geometry_msgs::msg::Pose pose_test;
    //     ros_message->message = &pose_test;
    //     cdr_deserializer->deserialize(serialized_message, general_type_support, ros_message);

    //   // write the content to the output file
    //   qDebug() << "POSE" << "," << pose_test.position.x << "," << pose_test.position.y << "," << pose_test.position.z << "," << pose_test.orientation.x << "," << pose_test.orientation.y << "," << pose_test.orientation.z << "," << pose_test.orientation.w;
    // }

    }
    

private:
    rosbag2_cpp::readers::SequentialReader reader_;
    std::map<std::string, std::string> topic_name_map_;

    std::string bag_path_;

    std::shared_ptr<rosbag2_storage::SerializedBagMessage> msg_;

    rosbag2_storage::StorageOptions storage_options_;
    rosbag2_cpp::ConverterOptions converter_options_;

    rosbag2_cpp::SerializationFormatConverterFactory factory_;
};

/*
CSV

    QTextStream csvStream(&csvFile);

    std::vector<rosbag2_storage::TopicMetadata> topics_and_types_ = reader.get_all_topics_and_types();
    std::map<std::string,std::string> topicNameMap;

    for (const auto & topic : topics_and_types_)
    {
        qDebug() << "TEST FOR EXPORT - TODO: CANCEL THESE PRINTS";
        qDebug() << "meta name: " << QString::fromStdString(topic.name);
        qDebug() << "meta type: " << QString::fromStdString(topic.type);
        qDebug() << "meta serialization_format: " << QString::fromStdString(topic.serialization_format);
        topicNameMap[topic.name]=topic.type;
    }

    auto ros_message = std::make_shared<rosbag2_cpp::rosbag2_introspection_message_t>();
    rosbag2_cpp::SerializationFormatConverterFactory factory;

    std::shared_ptr<rosbag2_storage::SerializedBagMessage> serialized_message;

    auto library_test = rosbag2_cpp::get_typesupport_library("geometry_msgs/msg/Pose", "rosidl_typesupport_cpp");
    auto type_support_pose = rosbag2_cpp::get_typesupport_handle("geometry_msgs/msg/Pose", "rosidl_typesupport_cpp", library_test);


    std::unique_ptr<rosbag2_cpp::converter_interfaces::SerializationFormatDeserializer> cdr_deserializer;
    cdr_deserializer = factory.load_deserializer("cdr");



*/