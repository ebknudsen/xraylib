/*
	XrayLib.NET copyright (c) 2010 Matthew Wormington. All rights reserved.

	File: Compound.h
	Author: Matthew Wormington
	Language: C++/CLI   
	Compiler: Microsoft Visual Studio 2008
	Created: September 5, 2010
	$Version:$
	$Revision:$
	$RevDate:$

	Description:
	Contains a managed class around the XrayLib CompoundParser for 
	use with XrayLib.NET.

	"A library for X-ray�matter interaction cross sections for
	X-ray fluorescence applications".
	A. Brunetti, M. Sanchez del Rio, B. Golosio, A. Simionovici, A. Somogyi, 
	Spectrochimica Acta Part B 59 (2004) 1725�1731
	http://ftp.esrf.fr/pub/scisoft/xraylib/

	Notes:
	A singleton pattern has been used so that only one instance of the class is ever
	created. The Instance property provides a global point of access to the instance.
	The implementation is based on the Static Initialization example in the following
	Microsoft article: http://msdn.microsoft.com/en-us/library/ms998558.aspx


	XrayLib copyright (c) 2009, Bruno Golosio, Antonio Brunetti, Manuel Sanchez del 
	Rio, Tom Schoonjans and Teemu Ikonen. All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright notice, this 
	  list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright notice, this 
  	  list of conditions and the following disclaimer 
	  in the documentation and/or other materials provided with the distribution.
	* The names of the contributors may not be used to endorse or promote products 
	  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY Bruno Golosio, Antonio Brunetti, Manuel Sanchez del Rio, 
	Tom Schoonjans, Teemu Ikonen and Matthew Wormington ''AS IS'' AND ANY EXPRESS OR IMPLIED 
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Bruno Golosio, Antonio 
	Brunetti, Manuel Sanchez del Rio, Tom Schoonjans and Teemu Ikonen BE LIABLE FOR ANY 
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
	OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
	OF SUCH DAMAGE.
*/

#include "..\XrayLib\xraylib-parser.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace System::Text;

/// <summary>
/// A namespace that contains scientific classes.
/// </summary>
namespace Science {

	/// <summary>
	/// A simple class that gives information on the composition of a compound. 
	/// </summary>
	public ref class CompoundData
	{		
		int _atomCount;
		int _elementCount;
		List<int>^ _atomicNumber;  
		List<double>^ _massFraction;  

	public:

		/// <summary> Default constructor. </summary>
		CompoundData()
		{
			_atomicNumber = gcnew List<int>();	
			_massFraction = gcnew List<double>();	
			Clear();
		}

		/// <summary>Constructor. </summary>
		/// <param name="compound">	The chemical formula of the compound. </param>
		CompoundData(String^ compound)
		{
			_atomicNumber = gcnew List<int>();	
			_massFraction = gcnew List<double>();	
			Clear();
			if (compound != nullptr)
				Parse(compound);
		}

		/// <summary> Parses the chemical formula of the compound into its component elements. </summary>
		/// <param name="compound">	The chemical formula of the compound. </param>
		void Parse(String^ compound)
		{
			struct ::compoundData cd;
			int validCompound;

			if (String::IsNullOrEmpty(compound))
				return;

			IntPtr p = Marshal::StringToHGlobalAnsi(compound);
			try
			{					
				char* pCompound = static_cast<char*>(p.ToPointer());
				validCompound = ::CompoundParser(pCompound, &cd);
			}
			finally
			{
				Marshal::FreeHGlobal(p);
			}
			
			if (validCompound != 0)
			{
				_atomCount = cd.nAtomsAll;
				_elementCount = cd.nElements;
				if (_elementCount > 0)
				{
					for (int i = 0; i < _elementCount; i++)
					{
						_atomicNumber->Add(cd.Elements[i]);
						_massFraction->Add(cd.massFractions[i]);
					}
				}
			}
						
			::_free_compound_data(&cd); 
		}

		/// <summary> Clears this object to its blank/initial state. </summary>
		void Clear()
		{
			_atomCount = 0;
			_elementCount = 0;
			_atomicNumber->Clear();
			_massFraction->Clear();
		}

		/// <summary>Gets the number of atoms. </summary>
		/// <value>	The number of atoms. </value>
		property int AtomCount
		{
			int get()
			{
				return _atomCount;
			}
		}

		/// <summary>	Gets the number of elements. </summary>
		/// <value>	The number of elements. </value>
		property int ElementCount
		{
			int get()
			{
				return _elementCount;
			}
		}

		/// <summary>Gets the atomic number of the component element with the specified index. </summary>
		/// <value>	The atomic number[int]. </value>
		property int AtomicNumber[int]
		{
			int get(int index)
			{
				return _atomicNumber[index];
			}
		}

		/// <summary>Gets the mass fraction of the component element with the specified index. </summary>
		/// <value>	The mass fraction[int]. </value>
		property double MassFraction[int]
		{
			double get(int index)
			{
				return _massFraction[index];
			}
		}

		/// <summary>Convert this object into a string representation. </summary>
		/// <returns>Null if it fails, else a string representation of this object. </returns>
		virtual String^ ToString() override 
		{
			StringBuilder^ sb = gcnew StringBuilder();
			sb->AppendLine(String::Format("AtomCount: {0}", _atomCount));
			sb->AppendLine(String::Format("ElementCount: {0}", _elementCount));
			sb->AppendLine(String::Format("AtomicNumber, MassFraction"));
			for (int i=0; i<_elementCount; i++)
				sb->AppendLine(String::Format("{0}, {1}", _atomicNumber[i], _massFraction[i]));	
			
			return sb->ToString();
		}
	};
}