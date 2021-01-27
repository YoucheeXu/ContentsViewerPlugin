## Python Script Instruction

### thePlugin Module

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">logInfo (msg) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] msg </td>
			<td colspan="2" >string to be recorded</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">save msg as information to log file</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">logWarn (msg) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] msg </td>
			<td colspan="2" >string to be recorded</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">save msg as warnning to log file</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">logErr (msg) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] msg </td>
			<td colspan="2" >string to be recorded</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">save msg as error to log file</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">fetch_document ( )</td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">string to be fetched</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">receives the full document from Npp</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">update_document (doc) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] doc </td>
			<td colspan="2" >string to be updated</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">update doc to Npp</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">fetch_selection ( )</td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">string to be fetched</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">receives the selection from Npp</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">update_selection (doc) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] sel </td>
			<td colspan="2" >selection to be updated to Npp</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">replace selection of Npp with sel</td>
		</tr>
	</tbody>
</table>

### theCViewerDlg Module

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">update_contentsview () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">update contents from theDocument Module to contentsListView of dlg</td>
		</tr>
	</tbody>
</table>

### theDocument Module

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">get_document ( )</td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">string to be got</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">get document from theDocument Module</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">set_document (doc) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] doc </td>
			<td colspan="2" >string to be set</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3" >NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">set document to theDocument Module</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">parse () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">number of item be indexed</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">parse document in theDocument Module using [Content] item in ContentsViewerPlugin.ini</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">headParse () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">number of item be indexed</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">parse head line of document in theDocument Module</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">specificParse (index) </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td>[in] index </td>
			<td colspan="2" >string to be indexed</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">number of item be indexed</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">parse document in theDocument Module using index in regular expression mode</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">merge_paragraphs () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">merge invalid paragraphs in theDocument Module</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">default_replace () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">replace the document in theDocument Module with stdRep.jb</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">trim_Contents () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">omit the leading and trailing whitespace of contens in theDocument Module</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" colspan="4" style="background:LightYellow;">number_contents () </td>
		<tr>
			<td width="10%" style="background:LightGreen">Parameter:</td>
			<td colspan="3">NULL</td>
		</tr>
		<tr>
			<td width="10%" style="background:khaki">Return:</td>
			<td colspan="3">NULL</td>
		</tr>		
		<tr>
			<td colspan="4" style="background:#E8F8E8;">convert Chinese numerals in contens to Arabic numerals in theDocument Module</td>
		</tr>
	</tbody>
</table>