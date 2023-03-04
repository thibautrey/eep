# Define function to get center coordinates of FOV
def get_fov_center():
    # Set up Astrometry.net
    api_key = "your_api_key_here"
    client = Client(api_key=api_key)
    field_radius = 1.0
    downsample_factor = 2
    
    # Capture image
    image = capture_image()
    
    # Download and solve image with Astrometry.net
    image_file = "image.fits"
    image_data = np.array(Image.fromarray(image).convert('L'))
    image_hdu = fits.PrimaryHDU(image_data)
    image_hdu.writeto(image_file, overwrite=True)
    job_id = client.upload(image_file, publicly_visible='n')
    submission_id = client.submit_job(job_id=job_id, scale_units='arcsecperpix',
                                      scale_lower=0.5, scale_upper=10.0,
                                      downsample_factor=downsample_factor,
                                      allow_commercial_use='n',
                                      allow_modifications='n')
    result = client.wait_for_job(submission_id=submission_id, timeout=600)
    wcs_header = WCS(result['wcs_header'])
    
    # Get FOV center coordinates
    fov_center = wcs_header.wcs_pix2world(image.shape[1] / 2, image.shape[0] / 2, 0)
    
    return fov_center

# Define function to display text on image
def display_text(text):
    font = ImageFont.truetype('arial.ttf', 16)
    draw.text((10, 10), text, font=font, fill=(255, 255, 255))
    return display
