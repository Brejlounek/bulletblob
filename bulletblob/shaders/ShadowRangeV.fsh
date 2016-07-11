#version 330

in vec2 UV;

out vec3 color;

uniform sampler2D tex;
uniform vec2 pixel;
 
void main(){

	vec3 col0=texture( tex, UV+vec2(0,-pixel.y*2) ).rgb;
	vec3 col1=texture( tex, UV+vec2(0,-pixel.y) ).rgb;
	vec3 col2=texture( tex, UV+vec2(0,0) ).rgb;
	vec3 col3=texture( tex, UV+vec2(0,pixel.y) ).rgb;
	vec3 col4=texture( tex, UV+vec2(0,pixel.y*2) ).rgb;

	float mincol = 0.028*col0.r + 0.236*col1.r + 0.472*col2.r + 0.236*col3.r + 0.028*col4.r;
	if (col0.b==0 && col1.b==0 && col2.b==0 && col3.b==0 && col4.b==0)
	{
		color = vec3(mincol, mincol, 0);	
	}
	float maxcol = 0.028*col0.b + 0.236*col1.b + 0.472*col2.b + 0.236*col3.b + 0.028*col4.b;

	float diff = maxcol - mincol;
	float th = 0.0013;
	if (diff<th)
		color = vec3( mincol, mincol, 0 );
	else
	{
		float sum = 0;
		float l0 = abs(col0.r*0.5 + col0.g*0.5 - maxcol);
		if (l0<th)
			sum += 0.028*col0.b;
		float l1 = abs(col1.r*0.5 + col1.g*0.5-maxcol);
		if (l1<th)
			sum += 0.236*col1.b;
		float l2 = abs(col2.r*0.5 + col2.g*0.5-maxcol);
		if (l2<th)
			sum += 0.472*col2.b;
		float l3 = abs(col3.r*0.5 + col3.g*0.5-maxcol);
		if (l3<th)
			sum += 0.236*col3.b;
		float l4 = abs(col4.r*0.5 + col4.g*0.5-maxcol);
		if (l4<th)
			sum += 0.028*col4.b;

		float ratio = sum;
		color = vec3( mincol, maxcol, ratio);
	}

















	/*float col0=texture( tex, UV+vec2(-pixel.x,-pixel.y) ).r;
	float col1=texture( tex, UV+vec2(0,-pixel.y*2) ).r;
	float col2=texture( tex, UV+vec2(pixel.x,-pixel.y) ).r;
	float col3=texture( tex, UV+vec2(-pixel.x*2,0) ).r;
	float col4=texture( tex, UV+vec2(0,0) ).r;
	float col5=texture( tex, UV+vec2(pixel.x*2,0) ).r;
	float col6=texture( tex, UV+vec2(-pixel.x,pixel.y) ).r;
	float col7=texture( tex, UV+vec2(0,pixel.y*2) ).r;
	float col8=texture( tex, UV+vec2(pixel.x,pixel.y) ).r;

	float mincol = col0;
	mincol = min(mincol, col1);
	mincol = min(mincol, col2);
	mincol = min(mincol, col3);
	mincol = min(mincol, col4);
	mincol = min(mincol, col5);
	mincol = min(mincol, col6);
	mincol = min(mincol, col7);
	mincol = min(mincol, col8);

	float maxcol = col0;
	maxcol = max(maxcol, col1);
	maxcol = max(maxcol, col2);
	maxcol = max(maxcol, col3);
	maxcol = max(maxcol, col4);
	maxcol = max(maxcol, col5);
	maxcol = max(maxcol, col6);
	maxcol = max(maxcol, col7);
	maxcol = max(maxcol, col8);

	float diff = maxcol - mincol;
	float th = 0.013;
	if (diff<th)
		color = vec3( col0, col0, 0 );
	else
	{
		float sum = 0;
		float l0 = abs(col0-maxcol);
		if (l0<th)
			sum += 1.0f/9;
		float l1 = abs(col1-maxcol);
		if (l1<th)
			sum += 1.0f/9;
		float l2 = abs(col2-maxcol);
		if (l2<th)
			sum += 1.0f/9;
		float l3 = abs(col3-maxcol);
		if (l3<th)
			sum += 1.0f/9;
		float l4 = abs(col4-maxcol);
		if (l4<th)
			sum += 1.0f/9;
		float l5 = abs(col5-maxcol);
		if (l5<th)
			sum += 1.0f/9;
		float l6 = abs(col6-maxcol);
		if (l6<th)
			sum += 1.0f/9;
		float l7 = abs(col7-maxcol);
		if (l7<th)
			sum += 1.0f/9;
		float l8 = abs(col8-maxcol);
		if (l8<th)
			sum += 1.0f/9;

		float ratio = sum;
		color = vec3( mincol, maxcol, ratio);
	}*/
}