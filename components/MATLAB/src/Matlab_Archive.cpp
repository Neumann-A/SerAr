///-------------------------------------------------------------------------------------------------
// file:	Matlab_Archive.cpp
//
// summary:	Implements the matlab archive class
///-------------------------------------------------------------------------------------------------

#include "Matlab_Archive.h"

namespace Archives 
{
    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Constructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///
    /// <param name="fpath">  	The filepath to the MATLAB file. </param>
    /// <param name="options">	(Optional) options for controlling the operation. </param>
    ///-------------------------------------------------------------------------------------------------
    MatlabOutputArchive::MatlabOutputArchive(const std::filesystem::path &fpath, const MatlabOptions &options)
        : OutputArchive(this), m_filepath(fpath), m_options(options), m_MatlabFile(getMatlabFile(fpath, options)) {};

    ///-------------------------------------------------------------------------------------------------
    /// <summary>	Destructor. </summary>
    ///
    /// <remarks>	Neumann, 17.02.2017. </remarks>
    ///-------------------------------------------------------------------------------------------------
    MatlabOutputArchive::~MatlabOutputArchive()
    {
        //Cleanup
        while (!Fields.empty())
        {
            mxDestroyArray(std::get<1>(Fields.top()));
            Fields.pop();
        }
        matClose(&m_MatlabFile);
    };

    void MatlabOutputArchive::finishMATLABArray()
    {
        if (Fields.empty())
            return;

        //assert(!Fields.empty());// , "Trying to pop more mxArrays from the stack than had been pushed"); //Programming error!

        auto TopField = Fields.top(); //Thats the field we have to add! (Child mxArray)
        Fields.pop(); //Remove it

        if (Fields.empty()) //at the bottom lvl; write array to mat
        {
            int status = matPutVariable(&m_MatlabFile, std::get<0>(TopField).c_str(), std::get<1>(TopField)); // This throws an expection and catches it itself!
            if (status != 0)
                throw std::runtime_error{ "Unable to write Array to MATLAB file! (Out of Memory?)" };
            mxDestroyArray(std::get<1>(TopField)); //release array and automaticlly destroys all arrays which have been added into the array!
        }
        else // we are recursive
        {
            auto& BottomField = Fields.top();
            auto& arr = std::get<1>(BottomField); //Parent mxArray
    
            if (mxIsStruct(arr))
            {
                auto index = mxAddField(arr, std::get<0>(TopField).c_str());

                if (index == -1)
                    throw std::runtime_error{ "Could not add Field to MATLAB struct! (Out of Memory?)" };

                mxSetFieldByNumber(arr, 0, index, std::get<1>(TopField)); // Add Child to parent
                                                                          //no destruction of the TopField array here!
            }
            else if (mxIsCell(arr))
            {
                std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
                //TODO: Implement mxArray insert for cell arrays;
                assert(false);// , "Case (Cell) not handeled by Archive currently!");
            }
            else if (mxIsNumeric(arr))
            {
                std::cout << "MATLAB case not handled. Debug me or add correct case" << std::endl;
                //TODO: Implement mxArray insert for numeric arrays;
                assert(false);// , "Case (Numeric) not handeled by Archive currently!");
            }
            else
            {
                std::cout << "Unknown MATLAB case. Debug me or add correct case" << std::endl;
                assert(true);// , "Case (Unknown) not handeled by Archive!");
            }
        }
    }

    template void MatlabOutputArchive::save<bool>(const Archives::NamedValue<bool>& value);
    template void MatlabOutputArchive::save<short>(const Archives::NamedValue<short>& value);
    template void MatlabOutputArchive::save<int>(const Archives::NamedValue<int>& value);
    //template void MatlabOutputArchive::save<long>(const Archives::NamedValue<long>& value);
    template void MatlabOutputArchive::save<long long>(const Archives::NamedValue<long long>& value);
    template void MatlabOutputArchive::save<unsigned int>(const Archives::NamedValue<unsigned int>& value);
    //template void MatlabOutputArchive::save<unsigned long>(const Archives::NamedValue<unsigned long>& value);
    template void MatlabOutputArchive::save<unsigned long long>(const Archives::NamedValue<unsigned long long>& value);
    template void MatlabOutputArchive::save<float>(const Archives::NamedValue<float>& value);
    template void MatlabOutputArchive::save<double>(const Archives::NamedValue<double>& value);
    template void MatlabOutputArchive::save<long double>(const Archives::NamedValue<long double>& value);
    template void MatlabOutputArchive::save<std::string>(const Archives::NamedValue<std::string>& value);
    template void MatlabOutputArchive::save<bool&>(const Archives::NamedValue<bool&>& value);
    template void MatlabOutputArchive::save<short&>(const Archives::NamedValue<short&>& value);
    template void MatlabOutputArchive::save<int&>(const Archives::NamedValue<int&>& value);
    //template void MatlabOutputArchive::save<long&>(const Archives::NamedValue<long&>& value);
    template void MatlabOutputArchive::save<long long&>(const Archives::NamedValue<long long&>& value);
    template void MatlabOutputArchive::save<unsigned int&>(const Archives::NamedValue<unsigned int&>& value);
    //template void MatlabOutputArchive::save<unsigned long&>(const Archives::NamedValue<unsigned long&>& value);
    template void MatlabOutputArchive::save<unsigned long long&>(const Archives::NamedValue<unsigned long long&>& value);
    template void MatlabOutputArchive::save<float&>(const Archives::NamedValue<float&>& value);
    template void MatlabOutputArchive::save<double&>(const Archives::NamedValue<double&>& value);
    template void MatlabOutputArchive::save<long double&>(const Archives::NamedValue<long double&>& value);
    template void MatlabOutputArchive::save<std::string&>(const Archives::NamedValue<std::string&>& value);
}
