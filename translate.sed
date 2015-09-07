s/［`_create_examination_string $1`］の開始・終了/Start or Stop [`_create_examination_string $1`]/
s/試験開始・終了/Start or Stop/
s/試験開始/Start/
s/試験終了/Stop/
s/終了/ closed/
s/受験者一覧表示・印刷/List applicants/
s/合格者一覧表示・印刷/List passers/
s/合格者登録/Register passers/
s/戻る/Back to previous menu/
s/日時:$1年$2月$3日/Date: $1 - $2 - $3/
s/教室:$4/Room: $4/
s/時限:$5限/Period: $5/
s/問題/Question/
s/現在の状態/Current State/
s/試験中（開始時刻 $START_TIME）/ in progress (start at $START_TIME)/
s/str=${str\/"-"\/"年"}//
s/str=${str\/"-"\/"月"}//
s/str=${str\/"-"\/"日 "}//
s/str=${str\/"-"\/" "}//
s/$str"限 "$question_name/$str $question_name/
s/通常方式のQuestionを番号で入力してください［1から$question_count_ordinaryを選択］/Question number for ordinary method [1 - $question_count_ordinary]/
s/カバー方式のQuestionを番号で入力してください［1から$question_count_coverを選択］/Question number for cover method [1 - $question_count_cover]/
s/$targetを出力するプリンタ名を入力してください/Printer name for $target/
s/＊PSファイルの生成に失敗しました/Error - fail to generate PS file/
s/［エンターキーを押してください］/[ Press Enter ]/
s/＊印刷に失敗しました/Error - fail to print/
s/＊不正なプリンタ名です/Error - invalid printer name/
s/試験監督お疲れ様でした/Thank you for administrating typing exam./
s/ict-foundation@sfc.keio.ac.jpに報告メールを送るのを忘れないでください！/Please send a report mail to ict-foundation@sfc.keio.ac.jp if you have any information./
s/新規の試験登録を開始します/Register a new examination/
s/教室の入力［1から5を選択］/Classroom [ 1 - 5 ]/
s/時限を番号で入力してださい［1から6を選択］/Period [ 1 - 6 ]/
s/1限/1st/
s/2限/2nd/
s/3限/3rd/
s/4限/4th/
s/5限/5th/
s/6限/6th/
s/Questionを乱数で選びますか？ /Do you want to choose question at random? /
s/はい/Yes/
s/いいえ/No/
s/上記の試験を登録してよろしいですか？/Are you sure to register? /
s/登録した試験の運営を行いますか？/Do you want to begin operation of the examination?/
s/＊その試験は既に登録されています/Error - the examination is already registered/
s/プログラムでファイルを書き込むことができません/Error - cannot write a file/
s/quota -v を実行してみてください．/Please check quota -v/
s/試験運営を実施する試験の番号を入力してください/Input a number of examination you want to operate/
s/メインメニューへ/Go to main menu/
s/［`_create_examination_string $1`］の試験運営/Operation of [ `_create_examination_string $1` ]/
s/実施する試験運営業務の番号を入力してください/Input a number of action you want to perform/
s/＊数値を入力してください/Error - input a number/
s/＊1か2の番号を入力してください/Error - input 1 or 2/
s/＊1から3の番号を入力してください/Error - input a number in 1 - 3/
s/＊1から5の番号を入力してください/Error - input a number in 1 - 5/
s/＊1から6の番号を入力してください/Error - input a number in 1 - 6/
s/＊1から$question_count_ordinaryの番号を入力してください/Error - input a number in 1 - $question_count_ordinary/
s/＊1から$question_count_coverの番号を入力してください/Error - input a number in 1 - $question_count_cover/
s/＊受験者が居ません/Error - No applicants/