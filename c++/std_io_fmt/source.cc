#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <unordered_map>


static const std::vector<std::string> kSubjectTable{
	"语文", "数学", "外语", "地理", "历史", "政治"
};

static const std::vector<std::string> kInvestigationObjs{
	"总分", "平均分"
};


static const char kESC{'q'};

static const size_t kSpace{8};

using SubjectScoreList = std::vector<std::pair<std::string, int>>;


int main(int argc, char** agrv) {

	std::unordered_map<std::string, SubjectScoreList> scoresheet{};
	size_t   max_name_length{15};
	
	std::cout << "录入学生成绩, 按 q 停止录入！" << std::endl;
	int studentnum{0};

	do {
		std::string studentname;
		std::cout << "输入第" <<studentnum <<"学生姓名, 按回车键确认！" << std::endl;
		std::cin >> studentname;
		
		if (kESC == studentname.front() && 1==studentname.size()) {
			std::cout << std:: endl;
			break;
		}

		if (!studentname.empty()) {
			max_name_length = studentname.size() > max_name_length ? studentname.size() : max_name_length;
			
			std::cout << "输入该学生各科成绩, 按回车键确认！" << std::endl;
			SubjectScoreList one_student_score_list;
			for (const auto& subject:kSubjectTable) {
				std::cout << subject<< ":";
				int  score{0};
				std::cin >> score;
				one_student_score_list.emplace_back(std::make_pair(subject, score));
			}
			scoresheet[studentname] = one_student_score_list;
			studentnum++;
		}
		
	} while (true);


	//print subject table line
	std::cout << std::setw(max_name_length+kSpace) << std::right <<"科目: ";
	for (const auto& subject: kSubjectTable) {
		std::cout  << subject << std::setw(kSpace);
	}

	for (const auto& statistic : kInvestigationObjs) {
		std::cout << statistic << std::setw(kSpace);
	}
	std::cout << std::endl;

	//print per student per subjet score
	for (const auto& student : scoresheet) {
		std::cout << std::setw(max_name_length) << std::left << student.first << std::setw(kSpace);


		int totalscore{0};
		for (const auto& sujects : student.second) {
			std::cout << std::left << sujects.second << std::setw(kSpace);
			totalscore += sujects.second;
		}
		std::cout << std::left << totalscore << std::setw(kSpace) << std::left << std::fixed << std::setprecision(2) << static_cast<float>(totalscore) / student.second.size();
		std::cout << std::endl;
	}
	return 0;
}


