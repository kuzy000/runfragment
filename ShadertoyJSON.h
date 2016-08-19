#pragma once

#include <rapidjson/document.h>

namespace RunFragment {

struct ShadertoyJSON {
	const std::string ver;
	
	struct Info {
		const std::string name;
		const std::string username;
		
		Info(const rapidjson::Document::ValueType& d)
			: name {d["name"].GetString()}
			, username {d["username"].GetString()} 
			{}
	} info;
	
	struct RenderPass {
		struct Input {
			const std::size_t id;
			const std::string src;
			const std::string ctype;
			const std::size_t channel;
			struct Sampler {
				const std::string vflip;
				Sampler(const rapidjson::Document::ValueType& d)
					: vflip {d["vflip"].GetString()} {}
			};
			const Sampler sampler;
			
			Input(const rapidjson::Document::ValueType& d)
				: id {d["id"].GetUint()}
				, src {d["src"].GetString()}
				, ctype {d["ctype"].GetString()}
				, channel {d["channel"].GetUint()}
				, sampler {d["sampler"]}
				{}
		};
		const std::vector<Input> inputs;
		
		struct Output {
			const std::size_t id;
			const std::size_t channel;
			
			Output(const rapidjson::Document::ValueType& d)
				: id {d["id"].GetUint()}
				, channel {d["channel"].GetUint()}
				{}
		};
		const std::vector<Output> outputs;
		
		const std::string code;
		const std::string name;
		const std::string type;
		
		RenderPass(const rapidjson::Document::ValueType& d)
			: inputs {
				[&d] {
					std::vector<Input> result;
					for(const auto& input : d["inputs"].GetArray()) {
						result.emplace_back(input);
					}
					return result;
				} ()
			}
			, outputs {
				[&d] {
					std::vector<Output> result;
					for(const auto& output : d["outputs"].GetArray()) {
						result.emplace_back(output);
					}
					return result;
				} ()
			}
			, code {d["code"].GetString()}
			, name {d["name"].GetString()}
			, type {d["type"].GetString()}
			{}
	};
	const std::vector<RenderPass> renderpass;
	
	ShadertoyJSON(const rapidjson::Document::ValueType& d)
		: ver {d["Shader"]["ver"].GetString()}
		, info {d["Shader"]["info"]}
		, renderpass {
			[&d] {
				std::vector<RenderPass> result;
				for(const auto& pass : d["Shader"]["renderpass"].GetArray()) {
					result.emplace_back(pass);
				}
				return result;
			} ()
		} {}
};

}
