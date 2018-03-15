#pragma once
#include "Import.hpp"
#include "ModelConfig.hpp"
#include "DataModel.hpp"
#include "Model.hpp"
#include "GeometricModel.hpp"
#include <boost/format.hpp>  
#include <boost/tokenizer.hpp>  
#include <boost/algorithm/string.hpp>  

class SemanticModel
	: public TransE
{
protected:
	vector<vec>	v_semantics;
	vector<vec> 	e_semantics;
	map<string,int> neighborNum;

protected:
	const double balance;
	int k=2,a=900;

public:
	SemanticModel(
		const Dataset& dataset,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file,
		int dim,
		double alpha,
		double training_threshold,
		double balance=0.1)
		:TransE(dataset, task_type, logging_base_path,dim, alpha, training_threshold),
		balance(balance)
	{
		logging.record() << "\t[Name]\tSemanticModel.TransE";
		logging.record() << "\t[Dimension]\t" << dim;
		logging.record() << "\t[Learning Rate]\t" << alpha;
		logging.record() << "\t[Training Threshold]\t" << training_threshold;
		logging.record() << "\t[Topic Model]\tLSI";
		logging.record() << "\t[Balance]\t" << balance;

		ifstream fin(semantic_file.c_str(), ios::binary);
		//storage_vmat<double>::load(v_semantics, fin);
		load_e_semantics();
		load_v_semantics();
		load_neighbor_num();
		cout << "File Loaded." << endl;
	}
	void load_neighbor_num(){
		int num_zero=0;
		ifstream fin;
		string neighborPath="./neighbor_num_delete.txt";
		fin.open(neighborPath.c_str());
		while(!fin.eof()){
			string entityName;
			int num;
			fin>>entityName;
			fin>>num;
			if(num==0)
				num_zero++;
			neighborNum.insert(make_pair(entityName,num));

		}
		cout<<"num_zero :"<<num_zero<<endl;
		fin.close();
		cout<<"success load entity neighbor num"<<endl;

	}
	void load_v_semantics(){
		vector<string> entityName;
		vector<vec> entityVec;
		string semantic_file="./lsi_semantic_delete/lsi_semantic_v1.txt";
		string entityPath="./data/FB15k_description/entity2id.txt";
		ifstream fin;
		fin.open(entityPath.c_str());
		while(!fin.eof()){

			string entityStr,id;
			fin>>entityStr>>id;
			entityName.push_back(entityStr);
		}
		fin.close();
		fin.open(semantic_file.c_str(), ios::binary);
		boost::char_separator<char> sep(" \t \"\',\\?!#%@");
		int index=0;
		entityVec.resize(entityName.size());
		while(!fin.eof()){


			vector<double> ve;
			string s;
			getline(fin,s);
			boost::tokenizer<boost::char_separator<char>>	token(s, sep);
			int k=0;
			for(auto i=token.begin(); i!=token.end();i++){

						string s=*i;
						double t=atof(s.c_str());
						ve.push_back(t);
			}
			vec v(ve);
			entityVec[index++]=v;
		}
		fin.close();
		v_semantics.resize(count_entity()+10);
		for (auto& elem : v_semantics)
		{
			elem = randn(dim);
		}
		for(int i=0;i<entityVec.size();i++){
			int entityId=0;
			bool find=false;
			for(auto j=data_model.entity_name_to_id.begin();
						j!=data_model.entity_name_to_id.end();j++){

					if(j->first==entityName[i]){
						entityId=j->second;
						find=true;
						break;

					}

			}
			if(find==true)
				v_semantics[entityId]=entityVec[i];
		}
		logging.record() << "description semantic file loaded";


	}
	void load_e_semantics(){

		e_semantics.resize(count_entity());
		for (auto& elem : e_semantics)
		{
			elem = randn(dim);
		}
		string semantic_file="./lsi_semantic_delete/lsi_entity_neighbor_delete.txt";
		ifstream fin;
		fin.open(semantic_file.c_str(), ios::binary);
		boost::char_separator<char> sep(" \t \"\',\\?!#%@");
		int index=0;
		while(!fin.eof()){


			vector<double> ve;
			string s;
			getline(fin,s);
			boost::tokenizer<boost::char_separator<char>>	token(s, sep);
			int k=0;
			for(auto i=token.begin(); i!=token.end();i++){

						string s=*i;
						double t=atof(s.c_str());
						ve.push_back(t);
			}
			vec v(ve);
			e_semantics[index++]=v;
		}
		fin.close();
		logging.record() << "neighbor semantic file loaded";
	}
	void write(vector<vec> v){

		ofstream fout;
		string path="./res";
		fout.open(path.c_str());
		for(auto ve:v){

			for(int i=0;i<50;i++)
				fout<<ve[i]<<" ";
			fout<<"\n";
		}
		fout.close();
	}
	SemanticModel(
		const Dataset& dataset,
		const TaskType& task_type,
		const string& logging_base_path,
		int dim,
		double alpha,
		double training_threshold,
		double balance = 0.1)
		:TransE(dataset, task_type, logging_base_path, dim, alpha, training_threshold),
		balance(balance)
	{
		logging.record() << "\t[Name]\tSemanticModel.TransE";
		logging.record() << "\t[Dimension]\t" << dim;
		logging.record() << "\t[Learning Rate]\t" << alpha;
		logging.record() << "\t[Training Threshold]\t" << training_threshold;
		logging.record() << "\t[Topic Model]\tLSI";
		logging.record() << "\t[V_semantic]\t random initialize";
		logging.record() << "\t[Balance]\t" << balance;

		v_semantics.resize(count_entity()+10);
		for (auto& elem : v_semantics)
		{
			elem = randn(dim);
		}

		e_semantics.resize(count_entity()+10);
		for (auto& elem : e_semantics)
		{
			elem = randn(dim);
		}

		load_neighbor_num();
	}

	SemanticModel(
		const Dataset& dataset,
		const string& file_zeroshot,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file,
		int dim,
		double alpha,
		double training_threshold,
		double balance = 0.1)
		:TransE(dataset, file_zeroshot, task_type,logging_base_path, dim, alpha, training_threshold),
		balance(balance)
	{
		logging.record() << "\t[Name]\tSemanticModel.TransE";
		logging.record() << "\t[Dimension]\t" << dim;
		logging.record() << "\t[Learning Rate]\t" << alpha;
		logging.record() << "\t[Training Threshold]\t" << training_threshold;
		logging.record() << "\t[Topic Model]\tLSI";
		logging.record() << "\t[Balance]\t" << balance;

		ifstream fin(semantic_file.c_str(), ios::binary);
		storage_vmat<double>::load(v_semantics, fin);
		fin.close();

		cout << "File Loaded." << endl;
	}

	SemanticModel(
		const Dataset& dataset,
		const string& file_zeroshot,
		const TaskType& task_type,
		const string& logging_base_path,
		int dim,
		double alpha,
		double training_threshold,
		double balance = 0.1)
		:TransE(dataset, file_zeroshot, task_type, logging_base_path, dim, alpha, training_threshold),
		balance(balance)
	{
		logging.record() << "\t[Name]\tSemanticModel.TransE";
		logging.record() << "\t[Dimension]\t" << dim;
		logging.record() << "\t[Learning Rate]\t" << alpha;
		logging.record() << "\t[Training Threshold]\t" << training_threshold;
		logging.record() << "\t[Topic Model]\tLSI";
		logging.record() << "\t[Balance]\t" << balance;

		v_semantics.resize(count_entity()+10);
		for (auto& elem : v_semantics)
		{
			elem = randn(dim);
		}

		cout << "File Loaded." << endl;
	}

public:
	vec semantic_composition_h(const pair<pair<int, int>, int>& triplet) const
	{
		string entityName=data_model.entity_id_to_name[triplet.first.first];
		int num = neighborNum.at(entityName);
		double t1 = 1/(1+pow(num,k)/a);
		double t2 = (pow(num,k)/a)*t1;
		return (t1*v_semantics[triplet.first.first] + t2*e_semantics[triplet.first.first])
					/ max(1e-5, sum(abs(t1*v_semantics[triplet.first.first] + t2*e_semantics[triplet.first.first])));
		return normalise(t1*v_semantics[triplet.first.first] + t2*e_semantics[triplet.first.first]);
	}
	vec semantic_composition_t(const pair<pair<int, int>, int>& triplet) const
	{
		string entityName=data_model.entity_id_to_name[triplet.first.second];
		int num = neighborNum.at(entityName);
		double t1 = 1/(1+pow(num,k)/a);
		double t2 = (pow(num,k)/a)*t1;
		return (t1*v_semantics[triplet.first.second] + t2*e_semantics[triplet.first.second])
			/ max(1e-5, sum(abs(t1*v_semantics[triplet.first.second] + t2*e_semantics[triplet.first.second])));
		return normalise(t1*v_semantics[triplet.first.second] + t2*e_semantics[triplet.first.second]);
	}
	virtual const vec semantic_composition(vec h,vec t) const
	{
		return (h + t)
			/ max(1e-5, sum(abs(h + t)));
		return normalise(h + t);
	}
	virtual double prob_triplets(const pair<pair<int, int>, int>& triplet)
	{
		vec semantic_v1 = semantic_composition_h(triplet);
		vec semantic_v2 = semantic_composition_t(triplet);

		vec semantic = semantic_composition(semantic_v1,semantic_v2);

		vec error = embedding_entity[triplet.first.first]
			+ embedding_relation[triplet.second]
			- embedding_entity[triplet.first.second];

		return	- balance * sum(abs(error - as_scalar(semantic.t()*error)*semantic))
					- sum(abs(error));

	}

	virtual void train_triplet(const pair<pair<int, int>, int>& triplet)
	{

		vec& head = embedding_entity[triplet.first.first];
		vec& tail = embedding_entity[triplet.first.second];
		vec& relation = embedding_relation[triplet.second];

		pair<pair<int, int>, int> triplet_f;
		data_model.sample_false_triplet(triplet, triplet_f);

		if (prob_triplets(triplet) - prob_triplets(triplet_f) > training_threshold)
			return;

		vec& head_f = embedding_entity[triplet_f.first.first];
		vec& tail_f = embedding_entity[triplet_f.first.second];
		vec& relation_f = embedding_relation[triplet_f.second];

		vec semantic = semantic_composition_h(triplet); //ready to modify
		vec error = head + relation - tail;
		double projection = as_scalar(semantic.t()*error);
		vec grad = sign(error - projection * semantic);
		grad = grad - as_scalar(grad.t()*semantic)*semantic;

		head -= alpha * balance * grad + alpha * sign(error);
		tail += alpha * balance * grad + alpha * sign(error);
		relation -= alpha * balance * grad + alpha * sign(error);

		vec semantic_f = semantic_composition_h(triplet_f); // ready to modify
		vec error_f = head_f + relation_f - tail_f;
		double projection_f = as_scalar(semantic_f.t()*error_f);
		vec grad_f = sign(error_f - projection_f * semantic_f);
		grad_f = grad_f - as_scalar(grad_f.t()*semantic_f)*semantic_f;

		head_f += alpha * balance * grad_f + alpha * sign(error_f);
		tail_f -= alpha * balance * grad_f + alpha * sign(error_f);
		relation_f += alpha * balance * grad_f + alpha * sign(error_f);

		if (norm_L2(head) > 1.0)
			head = normalise(head);

		if (norm_L2(tail) > 1.0)
			tail = normalise(tail);

		if (norm_L2(relation) > 1.0)
			relation = normalise(relation);

		if (norm_L2(head_f) > 1.0)
			head_f = normalise(head_f);

		if (norm_L2(tail_f) > 1.0)
			tail_f = normalise(tail_f);
	}
	virtual void train_batch()
	{
		for(int k=0;k<batchSize;k++){
			pair<pair<int, int>,int> triplet;
			sample(triplet);
			vec& head = embedding_entity[triplet.first.first];
			vec& tail = embedding_entity[triplet.first.second];
			vec& relation = embedding_relation[triplet.second];

			pair<pair<int, int>, int> triplet_f;
			data_model.sample_false_triplet(triplet, triplet_f);

			if (prob_triplets(triplet) - prob_triplets(triplet_f) > training_threshold)
				continue;

			vec& head_f = embedding_entity[triplet_f.first.first];
			vec& tail_f = embedding_entity[triplet_f.first.second];
			vec& relation_f = embedding_relation[triplet_f.second];
			vec error = head + relation - tail;

			vec semantic_v1 = semantic_composition_h(triplet);
			vec semantic_v2 = semantic_composition_t(triplet);
			vec semantic = semantic_composition(semantic_v1,semantic_v2);
			double projection = as_scalar(semantic.t()*error);
			vec grad = sign(error - projection * semantic);
			grad = grad - as_scalar(grad.t()*semantic)*semantic;

			head -= alpha * balance * grad + alpha * sign(error);
			tail += alpha * balance * grad + alpha * sign(error);
			relation -= alpha * balance * grad + alpha * sign(error);
			vec error_f = head_f + relation_f - tail_f;

			vec semantic_v1_f = semantic_composition_h(triplet);
			vec semantic_v2_f = semantic_composition_t(triplet);

			vec semantic_f = semantic_composition(semantic_v1_f,semantic_v2_f);
			double projection_f = as_scalar(semantic_f.t()*error_f);
			vec grad_f = sign(error_f- projection_f * semantic_f);
			grad_f = grad_f - as_scalar(grad.t()*semantic_f)*semantic_f;

			head_f += alpha * balance * grad_f + alpha * sign(error_f);
			tail_f -= alpha * balance * grad_f + alpha * sign(error_f);
			relation_f += alpha * balance * grad_f + alpha * sign(error_f);

			if (norm_L2(head) > 1.0)
				head = normalise(head);

			if (norm_L2(tail) > 1.0)
				tail = normalise(tail);

			if (norm_L2(relation) > 1.0)
				relation = normalise(relation);

			if (norm_L2(head_f) > 1.0)
				head_f = normalise(head_f);

			if (norm_L2(tail_f) > 1.0)
				tail_f = normalise(tail_f);
		}
	}

public:
	virtual vec entity_representation(int entity_id) const override
	{
		return embedding_entity[entity_id];

		if (data_model.zeroshot_pointer + 10 < data_model.set_entity.size())
			return v_semantics[entity_id];
		else
			return join_cols(embedding_entity[entity_id], v_semantics[entity_id]);
	}

public:
	virtual void save(const string& filename) override
	{
		ofstream fout(filename, ios::binary);
		storage_vmat<double>::save(embedding_entity, fout);
		storage_vmat<double>::save(embedding_relation, fout);
		storage_vmat<double>::save(v_semantics, fout);
		fout.close();
	}

	virtual void load(const string& filename) override
	{
		ifstream fin(filename, ios::binary);
		storage_vmat<double>::load(embedding_entity, fin);
		storage_vmat<double>::load(embedding_relation, fin);
		storage_vmat<double>::load(v_semantics, fin);
		fin.close();
	}
};

class SemanticModel_Joint
	: public SemanticModel
{
protected:
	vector<vector<string>>			documents,documents_nei;
	map<string, vec>			topic_words,topic_words_nei;
	vector<string>				words,words_nei;

protected:
	const double factor;

public:
	SemanticModel_Joint(
		const Dataset& dataset,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file_raw,
		int dim,
		double alpha,
		double training_threshold,
		double balance,
		double factor)
		:SemanticModel(dataset, task_type, logging_base_path, dim, alpha, training_threshold, balance),
		factor(factor)
	{
		logging.record() << "\t[Name]\tSemanticModel.Joint";
		logging.record() << "\t[Factor]\t" << factor;
		logging.record() << "k "<<k<<"\t"<<"a "<<"\t"<<a;
		load_neighbor_documents(); //load neighbor document

		documents.resize(count_entity()+10);

		fstream fin(semantic_file_raw.c_str());
			boost::char_separator<char> sep(" \t \"\',.\\?!#%@");
			while (!fin.eof())
			{
				string strin;
				getline(fin, strin);
				boost::tokenizer<boost::char_separator<char>>	token(strin, sep);

				string entity_name;
				vector<string>	entity_description;
				for (auto i = token.begin(); i != token.end(); ++i)
				{
					if (i == token.begin())
					{
						entity_name = *i;
					}
					else
					{
						entity_description.push_back(*i);
						if (topic_words.find(*i) == topic_words.end())
						{
							topic_words[*i] = randu(dim);
							words.push_back(*i);
						}
					}
				}
				if(data_model.entity_name_to_id.find(entity_name)==data_model.entity_name_to_id.end())
						continue;
				else

					documents[data_model.entity_name_to_id.find(entity_name)->second] = entity_description;
			}
			fin.close();


		logging.record() << "\tsuccess load description documents\t";
		cout << "File Loaded." << endl;
	}
	void load_neighbor_documents(){

		documents_nei.resize(count_entity()+10);
		string path="./neighbor/entity2neighbor_delete.txt";
		ifstream fin;
		fin.open(path.c_str());
		boost::char_separator<char> sep(" \t \"\',.\\?!#%@");
		int line=0;
		while(!fin.eof()){
			string strin;
			getline(fin, strin);
			boost::tokenizer<boost::char_separator<char>>	token(strin, sep);
			vector<string>	neighbors;
			for (auto i = token.begin(); i != token.end(); ++i)
			{

					neighbors.push_back(*i);
					if (topic_words_nei.find(*i) == topic_words_nei.end())
					{
						topic_words_nei[*i] = randu(dim);
						words_nei.push_back(*i);
					}
			}

			documents_nei[line++] = neighbors;
		}
		int i=0;
		for(vector<string> v:documents_nei){

			if(v.size()==0)
				i++;
		}
		cout<<"the number of empty neighbor set "<<i<<endl;
		cout<<"line "<<line<<endl;
		logging.record() << "\tsuccess load neighbor documents\t" ;
		fin.close();

	}

	SemanticModel_Joint(
		const Dataset& dataset,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file,
		const string& semantic_file_raw,
		int dim,
		double alpha,
		double training_threshold,
		double balance,
		double factor)
		:SemanticModel(dataset, task_type, logging_base_path, semantic_file, dim, alpha, training_threshold, balance),
		factor(factor)
	{
		logging.record() << "\t[Name]\tSemanticModel.Joint";
		logging.record() << "\t[Factor]\t" << factor;

		documents.resize(count_entity() + 10);

		ifstream fin;
		fin.open(semantic_file_raw.c_str());
		boost::char_separator<char> sep(" \t \"\',.\\?!#%@");
		int line=0;
		while (!fin.eof())
		{
			string strin;
			getline(fin, strin);
			boost::tokenizer<boost::char_separator<char>>	token(strin, sep);

			string entity_name;
			vector<string>	entity_description;
			for (auto i = token.begin(); i != token.end(); ++i)
			{
				if (i == token.begin())
				{
					entity_name = *i;
				}
				else
				{
					entity_description.push_back(*i);
					if (topic_words.find(*i) == topic_words.end())
					{
						topic_words[*i] = randu(dim);
						words.push_back(*i);
					}
				}
			}
			line++;
			cout<<line<<endl;
			documents[data_model.entity_name_to_id.find(entity_name)->second] = entity_description;
		}
		fin.close();

		cout << "File Loaded." << endl;
	}

	SemanticModel_Joint(
		const Dataset& dataset,
		const string& file_zeroshot,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file,
		const string& semantic_file_raw,
		int dim,
		double alpha,
		double training_threshold,
		double balance,
		double factor)
		:SemanticModel(dataset, file_zeroshot, task_type, logging_base_path, semantic_file, dim, alpha, training_threshold, balance),
		factor(factor)
	{
		logging.record() << "\t[Name]\tSemanticModel.Joint";
		logging.record() << "\t[Factor]\t" << factor;

		documents.resize(count_entity() + 10);

		fstream fin(semantic_file_raw.c_str());
		boost::char_separator<char> sep(" \t \"\',.\\?!#%@");
		while (!fin.eof())
		{
			string strin;
			getline(fin, strin);
			boost::tokenizer<boost::char_separator<char>>	token(strin, sep);

			string entity_name;
			vector<string>	entity_description;
			for (auto i = token.begin(); i != token.end(); ++i)
			{
				if (i == token.begin())
				{
					entity_name = *i;
				}
				else
				{
					entity_description.push_back(*i);
					if (topic_words.find(*i) == topic_words.end())
					{
						topic_words[*i] = randu(dim);
						words.push_back(*i);
					}
				}
			}

			documents[data_model.entity_name_to_id.find(entity_name)->second] = entity_description;
		}
		fin.close();

		cout << "File Loaded." << endl;
	}

	SemanticModel_Joint(
		const Dataset& dataset,
		const string& file_zeroshot,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file_raw,
		int dim,
		double alpha,
		double training_threshold,
		double balance,
		double factor)
		:SemanticModel(dataset, file_zeroshot, task_type, logging_base_path, dim, alpha, training_threshold, balance),
		factor(factor)
	{
		logging.record() << "\t[Name]\tSemanticModel.Joint";
		logging.record() << "\t[Factor]\t" << factor;

		documents.resize(count_entity() + 10);

		fstream fin(semantic_file_raw.c_str());
		boost::char_separator<char> sep(" \t \"\',.\\?!#%@");
		while (!fin.eof())
		{
			string strin;
			getline(fin, strin);
			boost::tokenizer<boost::char_separator<char>>	token(strin, sep);

			string entity_name;
			vector<string>	entity_description;
			for (auto i = token.begin(); i != token.end(); ++i)
			{
				if (i == token.begin())
				{
					entity_name = *i;
				}
				else
				{
					entity_description.push_back(*i);
					if (topic_words.find(*i) == topic_words.end())
					{
						topic_words[*i] = randu(dim);
						words.push_back(*i);
					}
				}
			}

			documents[data_model.entity_name_to_id.find(entity_name)->second] = entity_description;
		}
		fin.close();

		cout << "File Loaded." << endl;
	}

public:
	virtual void train_topic()
	{
#pragma omp parallel for
		for (auto idoc = documents.begin(); idoc != documents.end(); ++idoc)
		{
			vec& v_doc = v_semantics[idoc - documents.begin()];
			vec v_doc_grad = zeros(dim);

			for (auto iword = idoc->begin(); iword < idoc->end(); ++iword)
			{
				vec& v_word = topic_words[*iword];
				v_doc_grad += alpha * as_scalar(1 - v_doc.t()*v_word) * v_word;
				v_word += alpha * as_scalar(1 - v_doc.t()*v_word) * v_doc;

				vec& v_word_ns = topic_words[words[rand() % words.size()]];
				v_doc_grad += alpha * as_scalar(0 - v_doc.t()*v_word_ns) * v_word_ns;
				v_word_ns += alpha * as_scalar(0 - v_doc.t()*v_word_ns) * v_doc;

				v_word_ns = normalise(v_word_ns);
				v_word = normalise(v_word);
			}

			v_doc += v_doc_grad;
			v_doc = normalise(v_doc);
		}
	}
	void train_topic_neighbor(){
		for (auto idoc = documents_nei.begin(); idoc != documents_nei.end(); ++idoc)
		{
			vec& v_doc = e_semantics[idoc - documents_nei.begin()];
			vec v_doc_grad = zeros(dim);

			for (auto iword = idoc->begin(); iword < idoc->end(); ++iword)
			{
				vec& v_word = topic_words_nei[*iword];
				v_doc_grad += alpha * as_scalar(1 - v_doc.t()*v_word) * v_word;
				v_word += alpha * as_scalar(1 - v_doc.t()*v_word) * v_doc;

				vec& v_word_ns = topic_words_nei[words_nei[rand() % words_nei.size()]];
				v_doc_grad += alpha * as_scalar(0 - v_doc.t()*v_word_ns) * v_word_ns;
				v_word_ns += alpha * as_scalar(0 - v_doc.t()*v_word_ns) * v_doc;

				v_word_ns = normalise(v_word_ns);
				v_word = normalise(v_word);
			}

			v_doc += v_doc_grad;
			v_doc = normalise(v_doc);
		}

	}

	virtual void train_triplet(const pair<pair<int, int>, int>& triplet)
	{
		vec& head = embedding_entity[triplet.first.first];
		vec& tail = embedding_entity[triplet.first.second];
		vec& relation = embedding_relation[triplet.second];
		vec& head_sem = v_semantics[triplet.first.first];
		vec& tail_sem = v_semantics[triplet.first.second];

		pair<pair<int, int>, int> triplet_f;
		data_model.sample_false_triplet(triplet, triplet_f);

		if (prob_triplets(triplet) - prob_triplets(triplet_f) > training_threshold)
			return;

		vec& head_f = embedding_entity[triplet_f.first.first];
		vec& tail_f = embedding_entity[triplet_f.first.second];
		vec& relation_f = embedding_relation[triplet_f.second];
		vec& head_sem_f = v_semantics[triplet_f.first.first];
		vec& tail_sem_f = v_semantics[triplet_f.first.second];

		vec semantic = semantic_composition_h(triplet);  //
		vec error = head + relation - tail;
		double projection = as_scalar(semantic.t()*error);
		double length = as_scalar(semantic.t()*semantic);
		vec grad = sign(error - projection * semantic);
		grad = grad - as_scalar(grad.t()*semantic)*semantic;

		head -= alpha * balance * grad + alpha * sign(error);
		tail += alpha * balance * grad + alpha * sign(error);
		relation -= alpha * balance * grad + alpha * sign(error);
		head_sem += alpha * balance * factor * projection
			* (sign(error - projection * semantic) -
			as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(head_sem + tail_sem));
		tail_sem += alpha * balance * factor 
			* (sign(error - projection * semantic) -
			as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(head_sem + tail_sem));

		vec semantic_f = semantic_composition_h(triplet_f);  //
		vec error_f = head_f + relation_f - tail_f;
		double projection_f = as_scalar(semantic_f.t()*error_f);
		double length_f = as_scalar(semantic_f.t()*semantic_f);
		vec grad_f = error_f - projection_f  * semantic_f;
		grad_f = grad_f - as_scalar(grad_f.t()*semantic_f)*semantic_f;

		head_f += alpha * balance * grad_f + alpha * sign(error_f);
		tail_f -= alpha * balance * grad_f + alpha * sign(error_f);
		relation_f += alpha * balance * grad_f + alpha * sign(error_f);
		head_sem_f -= alpha * balance * factor * projection_f 
			* (sign(error_f - projection_f * semantic_f) -
			as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(head_sem_f + tail_sem_f));
		tail_sem_f -= alpha * balance * factor * projection_f 
			* (sign(error_f - projection_f * semantic_f) -
			as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(head_sem_f + tail_sem_f));

		if (norm_L2(head) > 1.0)
			head = normalise(head);

		if (norm_L2(tail) > 1.0)
			tail = normalise(tail);

		if (norm_L2(relation) > 1.0)
			relation = normalise(relation);

		if (norm_L2(head_f) > 1.0)
			head_f = normalise(head_f);

		if (norm_L2(tail_f) > 1.0)
			tail_f = normalise(tail_f);

		head_sem = normalise(head_sem);
		tail_sem = normalise(tail_sem);
		head_sem_f = normalise(head_sem_f);
		tail_sem_f = normalise(tail_sem_f);
	}
	virtual void train_batch()
	{

		for(int k=0;k<batchSize;k++){

			pair<pair<int, int>,int> triplet;
			sample(triplet);

			vec& head = embedding_entity[triplet.first.first];
			vec& tail = embedding_entity[triplet.first.second];
			vec& relation = embedding_relation[triplet.second];
			vec& head_sem = v_semantics[triplet.first.first];
			vec& tail_sem = v_semantics[triplet.first.second];
			vec& head_sem_nei = e_semantics[triplet.first.first];
			vec& tail_sem_nei = e_semantics[triplet.first.second];

			pair<pair<int, int>, int> triplet_f;
			data_model.sample_false_triplet(triplet, triplet_f);

			if (prob_triplets(triplet) - prob_triplets(triplet_f) > training_threshold)
				continue;

			vec& head_f = embedding_entity[triplet_f.first.first];
			vec& tail_f = embedding_entity[triplet_f.first.second];
			vec& relation_f = embedding_relation[triplet_f.second];
			vec& head_sem_f = v_semantics[triplet_f.first.first];
			vec& tail_sem_f = v_semantics[triplet_f.first.second];
			vec& head_sem_nei_f = e_semantics[triplet_f.first.first];
			vec& tail_sem_nei_f = e_semantics[triplet_f.first.second];
			vec error = head + relation - tail;

			vec semantic_v1 = semantic_composition_h(triplet);
			vec semantic_v2 = semantic_composition_t(triplet);

			vec semantic = semantic_composition(semantic_v1,semantic_v2);
			double projection = as_scalar(semantic.t()*error);

			vec grad = sign(error - projection * semantic);
			grad = grad - as_scalar(grad.t()*semantic)*semantic;

			head -= alpha * balance * grad + alpha * sign(error);
			tail += alpha * balance * grad + alpha * sign(error);
			relation -= alpha * balance * grad + alpha * sign(error);
			head_sem += alpha * balance * factor * projection
				* (sign(error - projection * semantic) -
				as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(semantic_v1 + semantic_v2));
			tail_sem += alpha * balance * factor
				* (sign(error - projection * semantic) -
				as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(semantic_v1 + semantic_v2));
			head_sem_nei += alpha * balance * factor * projection
				* (sign(error - projection * semantic) -
				as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(semantic_v1 + semantic_v2));
			tail_sem_nei += alpha * balance * factor
				* (sign(error - projection * semantic) -
				as_scalar(semantic.t()*sign(error - projection * semantic)) * sign(semantic_v1 + semantic_v2));

			vec error_f = head_f + relation_f - tail_f;
			vec semantic_v1_f = semantic_composition_h(triplet_f);
			vec semantic_v2_f = semantic_composition_t(triplet_f);

			vec semantic_f = semantic_composition(semantic_v1_f,semantic_v2_f);
			double projection_f = as_scalar(semantic_f.t()*error_f);
			double length_f = as_scalar(semantic_f.t()*semantic_f);
			vec grad_f = error_f - projection_f  * semantic_f;
			grad_f = grad_f - as_scalar(grad_f.t()*semantic_f)*semantic_f;

			head_f += alpha * balance * grad_f + alpha * sign(error_f);
			tail_f -= alpha * balance * grad_f + alpha * sign(error_f);
			relation_f += alpha * balance * grad_f + alpha * sign(error_f);
			head_sem_f -= alpha * balance * factor * projection_f
				* (sign(error_f - projection_f * semantic_f) -
				as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(semantic_v1_f + semantic_v2_f));
			tail_sem_f -= alpha * balance * factor * projection_f
				* (sign(error_f - projection_f * semantic_f) -
				as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(semantic_v1_f + semantic_v2_f));
			head_sem_nei_f -= alpha * balance * factor * projection_f
				* (sign(error_f - projection_f * semantic_f) -
				as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(semantic_v1_f + semantic_v2_f));
			tail_sem_nei_f -= alpha * balance * factor * projection_f
				* (sign(error_f - projection_f * semantic_f) -
				as_scalar(semantic_f.t()*sign(error_f - projection_f * semantic_f)) * sign(semantic_v1_f + semantic_v2_f));

			if (norm_L2(head) > 1.0)
				head = normalise(head);

			if (norm_L2(tail) > 1.0)
				tail = normalise(tail);

			if (norm_L2(relation) > 1.0)
				relation = normalise(relation);

			if (norm_L2(head_f) > 1.0)
				head_f = normalise(head_f);

			if (norm_L2(tail_f) > 1.0)
				tail_f = normalise(tail_f);

			head_sem = normalise(head_sem);
			tail_sem = normalise(tail_sem);
			head_sem_nei = normalise(head_sem_nei);
			tail_sem_nei = normalise(tail_sem_nei);
			head_sem_f = normalise(head_sem_f);
			tail_sem_f = normalise(tail_sem_f);
			head_sem_nei_f = normalise(head_sem_nei_f);
			tail_sem_nei_f = normalise(tail_sem_nei_f);
		}
	}
	void out_description_vec(){

		string path="./transe_vector/description.vec";
		ofstream  fout;
		fout.open(path.c_str());
		for(vec ve:v_semantics){

			for(int j=0;j<100;j++)
				fout<<ve[j]<<"\t";
			fout<<"\n";
		}
		fout.close();

	}
	void out_neighbor_vec(){

		string path="./transe_vector/neighbor.vec";
		ofstream  fout;
		fout.open(path.c_str());
		for(vec ve:e_semantics){

			for(int j=0;j<100;j++)
				fout<<ve[j]<<"\t";
			fout<<"\n";
		}
		fout.close();

	}
	virtual void train(bool last_time = false) override
	{
		TransE::train(last_time);

		if (epos % 10 == 0){
			train_topic();
			train_topic_neighbor();
		}
		if(last_time){
			out_description_vec();
			out_neighbor_vec();
		}

	}
};

class SemanticModel_ZeroShot
	:public SemanticModel_Joint
{
protected:
	mat		mat_transfer;

public:
	SemanticModel_ZeroShot(
		const Dataset& dataset,
		const string& file_zeroshot,
		const TaskType& task_type,
		const string& logging_base_path,
		const string& semantic_file,
		const string& semantic_file_raw,
		int dim,
		double alpha,
		double training_threshold,
		double balance,
		double factor)
		:SemanticModel_Joint(dataset, file_zeroshot, task_type, logging_base_path, semantic_file,
			semantic_file_raw, dim, alpha, training_threshold, balance, factor),
			mat_transfer(dim, dim)
	{
		logging.record() << "\t[Name]\tZeroShot";
	}

public:
	virtual void train(bool last_time = false) override
	{
		SemanticModel_Joint::train(last_time);

		if (last_time)
		{
			mat_transfer = zeros(dim, dim);

			for (auto i = 0; i < data_model.zeroshot_pointer; ++i)
			{
				mat_transfer += embedding_entity[i] * v_semantics[i].t();
			}
			
			for (auto i = data_model.zeroshot_pointer; i < count_entity(); ++i)
			{
				embedding_entity[i] = mat_transfer * v_semantics[i];
				embedding_entity[i] = normalise(embedding_entity[i]);
			}
		}
	}
};
