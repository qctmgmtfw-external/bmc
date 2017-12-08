--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/WebPreviewer.java Wed Aug 31 20:34:16 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/WebPreviewer.java Thu Sep  1 09:54:11 2011
@@ -99,48 +99,21 @@
 					g.drawString(m_view.viewWidth() + " x " + m_view.viewHeight(), REFRESH_BUTTON_WEIGHT+10, REFRESH_BUTTON_HEIGHT);
 				
 				BufferedImage m_image = JViewerApp.getInstance().getPrepare_buf().getM_image();
-				if (m_view.viewWidth() == 1024) {
-					BufferedImage	pImage = scaleJ2D(m_image, appletSize.width, appletSize.height-30, null);
-					Rectangle full_view = new Rectangle(0,0, appletSize.width, appletSize.height-31); //315
-					Rectangle r = full_view.intersection(g.getClipBounds());
-					BufferedImage rgn = pImage.getSubimage(r.x, r.y, r.width, r.height);
-					g.drawImage(rgn, r.x+5, r.y+30, null);
-					
-				} else {
-					BufferedImage	pImage = scaleJ2D(m_image, appletSize.width, appletSize.height-30, null);
-					Rectangle full_view = new Rectangle(0,0, appletSize.width, appletSize.height-31); //315
-					Rectangle r = full_view.intersection(g.getClipBounds());
-					BufferedImage rgn = pImage.getSubimage(r.x, r.y, r.width, r.height);
-					g.drawImage(rgn, r.x+5, r.y+30, null);
-				}
-				
+				BufferedImage	pImage = reduce(m_image, appletSize.width, appletSize.height-30);
+				Rectangle full_view = new Rectangle(0,0, appletSize.width, appletSize.height-31); //315
+				Rectangle r = full_view.intersection(g.getClipBounds());
+				BufferedImage rgn = pImage.getSubimage(r.x, r.y, r.width, r.height);
+				g.drawImage(rgn, r.x+5, r.y+30, null);
 		}
 		catch (Exception e) 
 		{
 		}
 	}
-	public static BufferedImage scaleJ2D(BufferedImage srcImage, double xscale, double yscale, RenderingHints hints)
-	{
-		AffineTransform affineTransform = new AffineTransform();
-		affineTransform.scale(xscale, yscale);
-		
-		AffineTransformOp affineTransformOp = new AffineTransformOp(affineTransform, hints);
-		
-		int width = (int)((double)srcImage.getWidth() * xscale);
-		int height = (int)((double)srcImage.getHeight() * yscale);
-		BufferedImage dstImage = new BufferedImage(width, height, srcImage.getType());
-		
-		return  affineTransformOp.filter(srcImage, dstImage);
-	}
- 
-
-
-	public static BufferedImage reduce(BufferedImage srcImage, double xscale, double yscale)
+
+	public static BufferedImage reduce(BufferedImage srcImage, int dstWidth, int dstHeight)
 	{   
-		int w = (int)((double)srcImage.getWidth() * xscale); 
-		int h = (int)((double)srcImage.getHeight() * yscale);
-		Image rescaled = srcImage.getScaledInstance(w, h, Image.SCALE_AREA_AVERAGING);
-		BufferedImage result = new BufferedImage(w, h, BufferedImage.TYPE_INT_RGB);
+		Image rescaled = srcImage.getScaledInstance(dstWidth, dstHeight, Image.SCALE_AREA_AVERAGING);
+		BufferedImage result = new BufferedImage(dstWidth, dstHeight, BufferedImage.TYPE_INT_RGB);
 
 		Graphics2D g = result.createGraphics();   
 		g.drawImage(rescaled, 0, 0, null);   
@@ -148,16 +121,6 @@
 		return result;   
 	}  
 
-
-	public static BufferedImage scaleJ2D(BufferedImage srcImage, int dstWidth, int dstHeight, RenderingHints hints) 
-	{
-		float xscale = (float) dstWidth / (float) srcImage.getWidth();
-		float yscale = (float) dstHeight / (float) srcImage.getHeight();
-		
-	//	return scaleJ2D(srcImage, xscale, yscale, hints);
-		return reduce(srcImage, xscale, yscale);
-	} 
-	
 	/* start */
 	public void start()
 	{
