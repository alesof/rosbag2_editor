#pragma once

#include <rosbag2_cpp/reader.hpp>
#include <rosbag2_cpp/typesupport_helpers.hpp>
#include <rosbag2_cpp/readers/sequential_reader.hpp>
#include <rosidl_typesupport_introspection_cpp/message_introspection.hpp>
#include "rosx_introspection/ros_parser.hpp"
#include "rosx_introspection/ros_utils/ros2_helpers.hpp"
#include "fstream"
#include <regex>

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

    void openBag(bool verbose = false)
    {

        storage_options_.uri = bag_path_;

        try
        {
            reader_.open(storage_options_, converter_options_);

            if (verbose)
            {
                const auto metadata = reader_.get_metadata();
            }

            std::vector<rosbag2_storage::TopicMetadata> topics_and_types_ = reader_.get_all_topics_and_types();

            for (const auto & topic : topics_and_types_)
            {
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

    void resetBag()
    {
        reader_.close();
        openBag();
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

    void bag2csv(std::string filename, bool split_topics = true){

        std::ofstream file;
        std::unordered_map<std::string, std::ofstream> fileStreams;
        std::regex slash_regex("/");
        std::regex csv_regex(".csv");

        std::shared_ptr<rosbag2_storage::SerializedBagMessage> serialized_message;
        std::string topic_type;

        if(!split_topics){
            file.open(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file for writing: " << filename << std::endl;
                return;
            }

        }else{
            for(auto &topic : topic_name_map_){
                std::cout<<"Creating file for topic: "<<topic.first<<std::endl;
                std::ofstream file(std::regex_replace(filename, csv_regex, "") + "_" + std::regex_replace(topic.first, slash_regex, "") + ".csv");
                if (!file.is_open()) {
                    std::cerr << "Error opening file for writing: " << filename << std::endl;
                    return;
                }
                fileStreams[topic.first] = std::move(file);
            }
        }

        while(reader_.has_next()){

            RosMsgParser::ParsersCollection<RosMsgParser::ROS2_Deserializer> parser;
            serialized_message = reader_.read_next();

            try{
                topic_type = topic_name_map_.at(serialized_message->topic_name);
            }
            catch(const std::exception &e){
                std::cerr << "Error getting topic type: " << e.what() << std::endl;
            }

            // if(topic_type=="rcl_interfaces/msg/Log") continue;

            parser.registerParser("joint_state", RosMsgParser::ROSType(topic_type), RosMsgParser::GetMessageDefinition(topic_type));
            auto data = serialized_message->serialized_data->buffer;
            auto length = serialized_message->serialized_data->buffer_length;

            std::vector<uint8_t> buffer(data, data + length);
            auto flat_container = parser.deserialize("joint_state", RosMsgParser::Span<uint8_t>(buffer));
            
            if(!split_topics){
                for (auto& it : flat_container->value)
                {
                    // std::cout <<"it.first:"<< it.first << " >> it.second:" << it.second.convert<double>() << std::endl;
                    file << it.first << "," << it.second.convert<double>() << std::endl;
                }
                for (auto& it : flat_container->name)
                {
                    // std::cout <<"it.first:"<< it.first << " >> it.second:" << it.second << std::endl;
                    file << it.first << "," << it.second << std::endl;
                }
            }
            else{

                auto file_it = fileStreams.find(serialized_message->topic_name);
                if (file_it != fileStreams.end()) {
                    
                    if(file_it->second.tellp() == 0){ //check if file empty write header
                        for (auto& it : flat_container->value)
                        {
                            file_it->second << it.first << ",";
                        }
                        for (auto& it : flat_container->name)
                        {
                            file_it->second << it.first << ",";
                        }
                        file_it->second << std::endl;
                    }

                    // std::cout<<"Found file associated with: "<<serialized_message->topic_name<<std::endl;
                    for (auto& it : flat_container->value)
                    {
                        file_it->second << it.second.convert<double>() << ",";
                    }
                    for (auto& it : flat_container->name)
                    {
                        file_it->second << it.second << ",";
                    }
                    file_it->second << std::endl;
                    
                } else {
                    std::cerr << "File stream not found for: " << filename << std::endl;
                }

            }
        }

        if (!split_topics) {
            file.close();
        } else {
            for (auto &fileStream : fileStreams) {
                fileStream.second.close();
            }
        }

        resetBag();

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



    //OLD APPROACH BEFORE ROSXPARSER:
    // 1. Get typesupport for specific topic
    // 2. Deserialize message
    // 3. Write to CSV

    //     std::cout<<"Reading messages"<<std::endl;

    //     while(reader_.has_next()){

    //         serialized_message = reader_.read_next();

    //         try{
    //             std::cout<<"Getting topic type"<<std::endl;
    //             topic_type = topic_name_map_.at(serialized_message->topic_name);
    //         }
    //         catch(const std::exception &e){
    //             std::cerr << "Error getting topic type: " << e.what() << std::endl;
    //         }

    //         //TEST BECAUSE I KNOW THE TOPIC IS GEOMETRY_MSGS/POSE
    //         geometry_msgs::msg::Pose pose_test;
    //         ros_message->message = &pose_test;
            
    //         std::cout<<"Deserializing message"<<std::endl;
    //         auto library_test = rosbag2_cpp::get_typesupport_library(topic_type, "rosidl_typesupport_cpp");
    //         auto type_support_pose = rosbag2_cpp::get_typesupport_handle(topic_type, "rosidl_typesupport_cpp", library_test);
    //         try{
    //             cdr_deserializer->deserialize(serialized_message, type_support_pose, ros_message);
    //             std::cout << "POSE" << "," << pose_test.position.x << "," << pose_test.position.y << "," << pose_test.position.z << "," << pose_test.orientation.x << "," << pose_test.orientation.y << "," << pose_test.orientation.z << "," << pose_test.orientation.w << std::endl;
    //         }
    //         catch(const std::exception &e){
    //             std::cerr << "Error deserializing message: " << e.what() << std::endl;
    //         }
    //     }

    // // while (reader.has_next()) {
        
    // //     serialized_message = reader.read_next();

    // //     auto general_lib = rosbag2_cpp::get_typesupport_library(topicNameMap[serialized_message->topic_name], "rosidl_typesupport_cpp");
    // //     auto general_type_support = rosbag2_cpp::get_typesupport_handle(topicNameMap[serialized_message->topic_name], "rosidl_typesupport_cpp", general_lib);
        
    // //     geometry_msgs::msg::Pose pose_test;
    // //     ros_message->message = &pose_test;
    // //     cdr_deserializer->deserialize(serialized_message, general_type_support, ros_message);

    // //   // write the content to the output file
    // //   qDebug() << "POSE" << "," << pose_test.position.x << "," << pose_test.position.y << "," << pose_test.position.z << "," << pose_test.orientation.x << "," << pose_test.orientation.y << "," << pose_test.orientation.z << "," << pose_test.orientation.w;
    // // }
